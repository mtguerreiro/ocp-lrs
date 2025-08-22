
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlBoostEnergyMpc.h"
#include "fsbuckboostConfig.h"

#include "controller/controller.h"

#include "utils/dfilt.h"

#include "stdio.h"
#include "string.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    float C;
    float L;
    float dt;
    float il_lim;
    float alpha;
    float Ky;
    float K_dz_1;
    float K_dz_2;
    float filt_coef;
    float filt_en;
    float kd;
}ctlparams_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static ctlparams_t params = {
    .C = 100e-6,
    .L = 15e-6,
    .dt = 1.0f/100e3,
    .il_lim = 15.0f,
    .alpha = 1e6,
    .Ky = 123.48210247f,
    .K_dz_1 = 9.68565167e+02f,
    .K_dz_2 = 4.37193140e-02f,
    .filt_coef = 0.78f,
    .filt_en = 0,
    .kd = 0
};

static float vo_r = 0.0f;
static float il_r = 0.0f;

/*
 * y_1 and y_dot_1 are future samples of y and y_dot, used for delay
 * compensation.
 */
static float yr = 0.0f;
static float y = 0.0f, y_1 = 0.0f, y_dot = 0.0f, y_dot_1 = 0.0f;

static float v, v_1;
static float v_init;
static float dv;

static uint32_t first_enter = 0;

static float dv_min, dv_min_v, dv_min_z2;
static float dv_max, dv_max_v, dv_max_z2;

static float io_filt;

//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcInit(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    // uint32_t i;
    float duty;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    if(first_enter == 0 ){
        io_filt = m->io;
    }

    if( params.filt_en != 0 ) io_filt = dfiltExpMovAvg(m->io, io_filt, params.filt_coef);
    else io_filt = m->io;

    /* References */
    il_r = m->v_dc_out * io_filt / m->v_in;

    vo_r = r->v_out - params.kd * io_filt;

    yr = (1.f / 2.f) * params.C * vo_r * vo_r + (1.f / 2.f) * params.L * il_r * il_r;

    /* States */
    y = (1.f / 2.f) * params.C * m->v_dc_out * m->v_dc_out + (1.f / 2.f) * params.L * m->il * m->il;
    y_dot = m->v_in * m->il - m->v_dc_out * io_filt;

    if( first_enter == 0 ){
        first_enter = 1;

        o->u = (m->v_dc_out - m->v_in) / (m->v_dc_out);
        v_init = m->v_in / params.L * (m->v_in - (1.0f - o->u) * m->v_dc_out) / params.alpha;

        v_1 = v_init;
        y_1 = y;
        y_dot_1 = y_dot;
    }

    /* Delay compensation */
    y = y + params.dt * y_dot + params.alpha * params.dt * params.dt / 2.0f * v_1;
    y_dot = y_dot + params.alpha * params.dt * v_1;

    /* Determine bounds for dv */
    dv_min_v = m->v_in / params.L * (m->v_in - m->v_dc_out) / params.alpha - v_1;
    dv_max_v = m->v_in * m->v_in / params.L / params.alpha - v_1;

    dv_min_z2 = (-params.il_lim * m->v_in - m->v_dc_out * io_filt - 2.0f * y_dot + y_dot_1) / (params.alpha * params.dt);
    dv_max_z2 = ( params.il_lim * m->v_in - m->v_dc_out * io_filt - 2.0f * y_dot + y_dot_1) / (params.alpha * params.dt);

    dv_min = dv_min_v > dv_min_z2 ? dv_min_v : dv_min_z2;
    dv_max = dv_max_v < dv_max_z2 ? dv_max_v : dv_max_z2;

    /* Optimization */
    dv = - params.Ky * (y - yr) - params.K_dz_1 * (y - y_1) - params.K_dz_2 * (y_dot - y_dot_1);
    if( dv > dv_max ) dv = dv_max;
    else if( dv < dv_min ) dv = dv_min;
    v = v_1 + dv;

    /* Saves variables */
    y_1 = y;
    y_dot_1 = y_dot;
    v_1 = v;

    /* Feedback linearization */
    duty = 1.0f - 1.0f / m->v_dc_out * (m->v_in - params.L / m->v_in * params.alpha * v);
    
    if( duty > 1.0f ) duty = 1.0f;
    else if( duty < 0.0f ) duty = 0.0f;

    o->u = duty;

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcSetParams(void *buffer, uint32_t size){

    if( size != sizeof(ctlparams_t) ) return -1;
    memcpy((void *)&params, buffer, size);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcGetParams(void *buffer, uint32_t size){

    if( size < sizeof(ctlparams_t) ) return -1;

    memcpy(buffer, (void *)&params, sizeof(ctlparams_t));

    return sizeof(ctlparams_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostEnergyMpcReset(void){

    first_enter = 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcFirstEntry(void *meas, int32_t nmeas,
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
int32_t fsbuckboostControlBoostEnergyMpcLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    first_enter = 0;

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostEnergyMpcGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlBoostEnergyMpcInit;
    cbs->run = fsbuckboostControlBoostEnergyMpcRun;
    cbs->setParams = fsbuckboostControlBoostEnergyMpcSetParams;
    cbs->getParams = fsbuckboostControlBoostEnergyMpcGetParams;
    cbs->reset = fsbuckboostControlBoostEnergyMpcReset;
    cbs->firstEntry = fsbuckboostControlBoostEnergyMpcFirstEntry;
    cbs->lastExit = fsbuckboostControlBoostEnergyMpcLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
