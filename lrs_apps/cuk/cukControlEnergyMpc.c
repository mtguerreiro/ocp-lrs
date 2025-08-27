

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukControlEnergyMpc.h"

#include "ocpConfig.h"
#include "ocp/ocpTrace.h"

#include "cukConfig.h"

// #include "cdmpc/mvops.h"
// #include "cdmpc/dmpc_matrices.h"
// #include "cdmpc/dmpc_defs.h"

#include "controller/controller.h"

#include "string.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define L_PAST      200

typedef struct{

    float Ky;
    float K_dz_1;
    float K_dz_2;
    float dt;

    float alpha;

    float Co;

    float il_max;
    float il_min;

    float Ku_freq_1;
    float Ku_freq_2[L_PAST];
}params_t;

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
// #define Ky      (4.015134334564209f)
// #define K_dz_1  (114.55923461914062f)
// #define K_dz_2  (0.016813231632113457f)
//
// #define params.dt      ((float)(1.0f/100e3))
//
// #define params.alpha   ((float)1e6)
//
// #define params.Co      ((float)330e-6)
//
// #define params.il_max  6.0f
// #define params.il_min  0.15f

static float p_in = 0.0f, p_out = 0.0f;

/* y_1 and y_dot_1 are past samples of y and y_dot */
static float y = 0.0f, y_1 = 0.0f, y_dot = 0.0f, y_dot_1 = 0.0f;
static float yr = 0.0f;

static float v, v_1 = 0.0f;
static float dv;

static uint32_t first_enter = 0;

static float dv_min, dv_min_v, dv_min_z2;
static float dv_max, dv_max_v, dv_max_z2;

static float D[L_PAST] = {0.0f};

static params_t params;
static params_t params = {
    .Ky = 4.142886638641357f, .K_dz_1 = 108.40249633789062f, .K_dz_2 = 0.01472429372370243f,
    .dt=1.0f/100e3, .alpha = 1e6, .Co = CUK_CONFIG_C_O,
    .il_max = 6.0f, .il_min = 0.15f,
    .Ku_freq_1 = 0.0f, .Ku_freq_2 = {0.0f}
};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukControlEnergyMpcInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlEnergyMpcRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    float **p;
    cukConfigMeasurements_t *hwm;
    cukConfigSwMeasurements_t *swm;
    cukConfigReferences_t *r = (cukConfigReferences_t *)refs;
    cukConfigControl_t *o = (cukConfigControl_t *)outputs;

    static float x1, x2, x3, x4;
    static float e_x1, e_x2, e_x3, e_x4;
    static float x1_r, x2_r, x3_r, x4_r;
    static float e_x1_r, e_x2_r, e_x3_r, e_x4_r;
    uint32_t i;
    float duty;
    float aux = 0;
    float y_dot_comp;

    p = (float **)meas;

    hwm = (cukConfigMeasurements_t *)p[0];
    swm = (cukConfigSwMeasurements_t *)p[1];

    x1 = hwm->i1;
    x2 = hwm->i2;
    x3 = hwm->v1 + (1.0f / CUK_CONFIG_TF_N2N1) * hwm->v2;
    x4 = hwm->vo_dc;

    /* Energies */
    e_x1 = (0.5f) * CUK_CONFIG_L_IN * x1 * x1;
    e_x2 = (0.5f) * CUK_CONFIG_L_OUT * x2 * x2;
    e_x3 = (0.5f) * CUK_CONFIG_C_C * x3 * x3 * CUK_CONFIG_TF_N2N1_SQ / (CUK_CONFIG_TF_N2N1_SQ + 1.0f);
    e_x4 = (0.5f) * params.Co * x4 * x4;
    y = e_x1 + e_x2 + e_x3 + e_x4;

    /* Input, output and converter power */
    p_in = swm->pi;
    p_out = swm->po;
    y_dot = p_in - p_out;

    /* References */
    x4_r = r->vo;
    x1_r = p_out / hwm->vi_dc;
    x2_r = p_out / x4_r;
    x3_r = hwm->vi_dc + x4_r * (1.0f / CUK_CONFIG_TF_N2N1);

    e_x1_r = (0.5f) * CUK_CONFIG_L_IN * x1_r * x1_r;
    e_x2_r = (0.5f) * CUK_CONFIG_L_OUT * x2_r * x2_r;
    e_x3_r = (0.5f) * CUK_CONFIG_C_C * x3_r * x3_r * CUK_CONFIG_TF_N2N1_SQ / (CUK_CONFIG_TF_N2N1_SQ + 1.0f);
    e_x4_r = (0.5f) * params.Co * x4_r * x4_r;
    yr = e_x1_r + e_x2_r + e_x3_r + e_x4_r;

    /* Initialization */
    if( first_enter == 0 ){
        first_enter = 1;
        y_1 = y;
        v_1 = 0.0f;
    }

    /* Delay compensation */
    y_dot_comp = y_dot;
    y = y + params.dt * y_dot + params.alpha * params.dt * params.dt / 2.0f * v_1;
    y_dot = y_dot + params.alpha * params.dt * v_1;

    //y_dot_comp_1 = y_dot;
    //y_dot_comp = y_dot + params.alpha * params.dt * v_1;

    /* Determine bounds for dv */
    dv_min_v = hwm->vi_dc / CUK_CONFIG_L_IN * (hwm->vi_dc - x3) / params.alpha - v_1;
    dv_max_v = hwm->vi_dc * hwm->vi_dc / CUK_CONFIG_L_IN / params.alpha - v_1;

    dv_min_z2 = (params.il_min * hwm->vi_dc - p_out - 2.0f * y_dot + y_dot_comp) / (params.alpha * params.dt);
    dv_max_z2 = (params.il_max * hwm->vi_dc - p_out - 2.0f * y_dot + y_dot_comp) / (params.alpha * params.dt);

    dv_min = dv_min_v > dv_min_z2 ? dv_min_v : dv_min_z2;
    dv_max = dv_max_v < dv_max_z2 ? dv_max_v : dv_max_z2;

    /* Optimization */
    dv = - params.Ky * (y - yr) - params.K_dz_1 * (y - y_1) - params.K_dz_2 * (y_dot - y_dot_1);

    for(i = 0; i < L_PAST; i++) aux += params.Ku_freq_2[i] * D[i];
    dv = dv - params.Ku_freq_1 * v_1 - aux;

    if( dv > dv_max ) dv = dv_max;
    else if( dv < dv_min ) dv = dv_min;
    v = v_1 + dv;

    /* Saves variables */
    y_1 = y;
    y_dot_1 = y_dot;
    v_1 = v;

    /* Feedback linearization */
    duty = 1.0f - 1.0f / x3 * (hwm->vi_dc - CUK_CONFIG_L_IN / hwm->vi_dc * params.alpha * v);

    o->u = duty;

    if( o->u > 1.0f ) o->u = 1.0f;
    else if ( o->u < 0.0f ) o->u = 0.0f;

    for(i = 0; i < (L_PAST - 1); i++){
        D[i] = D[i + 1];
    }
    D[L_PAST - 1] = v_1;

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t cukControlEnergyMpcSetParams(void *buffer, uint32_t size){

    if( size != sizeof(params_t) ) return -1;

    memcpy( (void *)&params, buffer, size );

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlEnergyMpcGetParams(void *buffer, uint32_t size){

    if( size < sizeof(params_t) ) return -1;

    memcpy( buffer, (void *)&params, sizeof(params_t) );

    return sizeof(params_t);
}
//-----------------------------------------------------------------------------
int32_t cukControlEnergyMpcFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    return 0;
}
//-----------------------------------------------------------------------------
void cukControlEnergyMpcReset(void){

    first_enter = 0;
}
//-----------------------------------------------------------------------------
void cukControlEnergyMpcGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = cukControlEnergyMpcInitialize;
    cbs->run = cukControlEnergyMpcRun;
    cbs->setParams = cukControlEnergyMpcSetParams;
    cbs->getParams = cukControlEnergyMpcGetParams;
    cbs->reset = cukControlEnergyMpcReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
