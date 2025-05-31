
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlBoostEnergyMpc.h"
#include "fsbuckboostConfig.h"

#include "controller/controller.h"

#include "utils/dfilt.h"

#include "cdmpc/mvops.h"
#include "cdmpc/dmpc.h"
#include "cdmpc/dmpc_defs.h"
#include "cdmpc/dmpc_matrices.h"

#include "stdio.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================


//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
#define V_GAIN          ((float) 1e6 )

static float C = 100e-6;
static float L = 15e-6;

static float vo_r = 0.0f;
static float il_r = 0.0f;

static float yr = 0.0f;
static float y = 0.0f, y_dot = 0.0f;

//static float u = 0.0f, v = 0.0f;
static float v = 0.0f;

static float k1 = 7743839.640425906f, k2 = 5599.999998136721, k3 = -5375358561.238816;

static float dt = 1.0 / 100e3;

static float e = 0.0f, e_dot = 0.0f;

static float io_filt = 0.0f;

static float alpha = 0.78;
static uint32_t filt_en = 0;

static uint32_t first_enter = 0;

static float kd = 0.0f;

static float xm[DMPC_CONFIG_NXM] = {0};
static float xm_1[DMPC_CONFIG_NXM] = {0};

static float ref[DMPC_CONFIG_NY];

static float u[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0};
static float du[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0};

uint32_t n_iters;

static float u_init, u_min, u_max;
static float x_init, x_min, x_max;

#ifndef IL_LIM
#define IL_LIM  12.5f
#endif


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

    uint32_t i;
    float duty;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    if(first_enter == 0 ){
        io_filt = m->io;
    }

    if( filt_en != 0 ) io_filt = dfiltExpMovAvg(m->io, io_filt, alpha);
    else io_filt = m->io;

    /* References */
    il_r = m->v_dc_out * io_filt / m->v_in;

    vo_r = r->v_out - kd * io_filt;

    yr = (1. / 2.) * C * vo_r * vo_r + (1. / 2.) * L * il_r * il_r;

    /* States */
    y = (1. / 2.) * C * m->v_dc_out * m->v_dc_out + (1. / 2.) * L * m->il * m->il;
    y_dot = m->v_in * m->il - m->v_dc_out * io_filt;

    u_min = m->v_in / L * (m->v_in - m->v_dc_out) / V_GAIN;
    u_max = m->v_in * m->v_in / L / V_GAIN;

    x_min = -IL_LIM * m->v_in - m->v_dc_out * io_filt;
    x_max =  IL_LIM * m->v_in - m->v_dc_out * io_filt;


    if( first_enter == 0 ){
        first_enter = 1;

        x_init = y;
        o->u = (m->v_dc_out - m->v_in) / (m->v_dc_out);
        u_init = m->v_in / L * (m->v_in - (1.0f - o->u) * m->v_dc_out) / V_GAIN;
        v = u_init;

        xm_1[0] = x_init;
        u[0] = u_init;
    }

    /* Update bounds on U */
    DMPC_CONFIG_U_MIN[0] = u_min;
    DMPC_CONFIG_U_MAX[0] = u_max;

    /* Update bounds on X */
    DMPC_CONFIG_XM_MIN[0] = x_min;
    DMPC_CONFIG_XM_MAX[0] = x_max;

    /* Reference */
    ref[0] = yr;

    /* Assembles state vector */
    xm[0] = y;
    xm[1] = y_dot;

    /* Delay compensation */
    dmpcDelayComp(xm, xm, u);

    /* Optimization */
    dmpcOpt(xm, xm_1, ref, u, &n_iters, du);
    //n_iters_float = (float) n_iters;

    /* Computes u = du + u_1 */
    sumv(u, du, DMPC_CONFIG_NU, u);

    /* Saves variables */
    for(i = 0; i < DMPC_CONFIG_NXM; i++){
        xm_1[i] = xm[i];
    }

    v = u[0] * V_GAIN;

    /* Feedback linearization */
    duty = 1.0f - 1.0f / m->v_dc_out * (m->v_in - L / m->v_in * v);
    
    printf("%s: Duty: %f\n", __FUNCTION__, duty);
    fflush(stdout);

    if( duty > 1.0f ) duty = 1.0f;
    else if( duty < 0.0f ) duty = 0.0f;

    o->u = duty;

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcSetParams(void *params, uint32_t size){

    float *p = (float *)params;

    k1 = *p++;
    k2 = *p++;
    k3 = *p++;
    dt = *p++;

    C = *p++;
    L = *p++;

    alpha = *p++;
    filt_en = (uint32_t)*p++;

    kd = *p++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcGetParams(void *buffer, uint32_t size){

    float *p = (float *)buffer;

    *p++ = k1;
    *p++ = k2;
    *p++ = k3;
    *p++ = dt;

    *p++ = C;
    *p++ = L;

    *p++ = alpha;
    *p++ = (float)filt_en;

    *p++ = kd;

    return 36;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostEnergyMpcReset(void){

    e = 0.0f;

    first_enter = 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

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
