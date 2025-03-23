
#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostControlEnergyInt.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include "boostConfig.h"

#include "dfilt.h"

#include "dmpc.h"
#include "dmpc_defs.h"
#include "dmpc_matrices.h"
#include "mvops.h"

#include "boostHw.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void boostControlEnergyMpcEnableScaledInt(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
//#define IL_LIM          5
#define SW_DIV_FACTOR   4

#define V_GAIN          ((float) 1e6 )

static float C = 150e-6;
static float L = 47e-6;

static float vo_r = 0.0f;
static float il_r = 0.0f;

static float yr = 0.0f;
static float y = 0.0f, y_dot = 0.0f;

static float v = 0.0f, v_lin = 0.0f;

//static float k1 = 7743839.640425906f, k2 = 5599.999998136721, k3 = -5375358561.238816;
//
//static float dt = 1.0 / 100e3;
//
//static float e = 0.0f, e_dot = 0.0f;

static float xm[DMPC_CONFIG_NXM] = {0};
static float xm_1[DMPC_CONFIG_NXM] = {0};
//static float dx[DMPC_CONFIG_NXM] = {0};

static float ref[DMPC_CONFIG_NY];

static float u[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0};
//static float u_1[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0};
static float du[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0};

//static float aux1[DMPC_CONFIG_NXM], aux2[DMPC_CONFIG_NXM];

static uint32_t n_iters;
static float n_iters_float;

static uint32_t first_enter_lin = 0;
static uint32_t first_enter_mpc = 0;

static float duty = 0.0f;

static uint32_t div_count = 0;

static float alpha = 0.78;

static uint32_t filt_en = 0;

static float io_filt = 0.0f;

static float u_init, u_min, u_max;
static float x_init, x_min, x_max;

static float il_lim = 5.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostControlEnergyMpcInitialize(void){

    ocpTraceAddSignal(OCP_TRACE_1, (void *)&y, "Energy");
    ocpTraceAddSignal(OCP_TRACE_1, (void *)&yr, "Energy ref");

    ocpTraceAddSignal(OCP_TRACE_1, (void *)&io_filt, "Filt. output current");
    ocpTraceAddSignal(OCP_TRACE_1, (void *)&duty, "Duty");
}
//-----------------------------------------------------------------------------
int32_t boostControlEnergyMpcSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    L = *p++;
    C = *p++;

    alpha = *p++;
    filt_en = (uint32_t)*p++;

    il_lim = *p++;

	return 16;
}
//-----------------------------------------------------------------------------
int32_t boostControlEnergyMpcGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

    float *p = (float *)out;

    *p++ = L;
    *p++ = C;

    *p++ = alpha;
    *p++ = (float)filt_en;

    *p++ = il_lim;

    return 20;
}
//-----------------------------------------------------------------------------
int32_t boostControlEnergyMpcRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    boostConfigMeasurements_t *m = (boostConfigMeasurements_t *)meas;
    boostConfigReferences_t *r = (boostConfigReferences_t *)refs;
    boostConfigControl_t *o = (boostConfigControl_t *)outputs;

    if( first_enter_lin == 0 ) io_filt = m->i_o;

    if( filt_en != 0 ) io_filt = dfiltExpMovAvg(m->i_o, io_filt, alpha);
    else io_filt = m->i_o;

    /* References */
    il_r = m->v_out * io_filt / m->v_dc_in;

    vo_r = r->v_o;

    yr = (1. / 2.) * C * vo_r * vo_r + (1. / 2.) * L * il_r * il_r;

    /* States */
    y = (1. / 2.) * C * m->v_dc_out * m->v_dc_out + (1. / 2.) * L * m->i_l * m->i_l;
    y_dot = m->v_dc_in * m->i_l - m->v_dc_out * io_filt;

    u_min = m->v_dc_in / L * (m->v_dc_in - m->v_dc_out) / V_GAIN;
    u_max = m->v_dc_in * m->v_dc_in / L / V_GAIN;

    x_min = -il_lim * m->v_dc_in - m->v_dc_out * io_filt;
    x_max =  il_lim * m->v_dc_in - m->v_dc_out * io_filt;


    /* Uncomment for PiL */
//    if( first_enter_lin == 0 ){
//        first_enter_lin = 1;
//        x_init = y;
//        u_init = m->v_dc_in / L * (m->v_dc_in - (1.0f - o->u) * m->v_dc_out) / V_GAIN;
//    }
//    if( div_count == 0 ){
//        boostControlEnergyMpcOpt();
//    }
//
//    div_count++;
//    if( div_count == (SW_DIV_FACTOR) ){
//        div_count = 0;
//        v_lin = v;
//    }

    /* Uncomment for HW */
    if( first_enter_lin == 0 ){
        first_enter_lin = 1;
        x_init = y;
        u_init = m->v_dc_in / L * (m->v_dc_in - (1.0f - o->u) * m->v_dc_out) / V_GAIN;
        v_lin = u_init;
        v = u_init;
        boostControlEnergyMpcEnableScaledInt();
    }
    else{
        div_count++;
        if( div_count == (SW_DIV_FACTOR - 1) ) v_lin = v;
        if( div_count == (SW_DIV_FACTOR) ) div_count = 0;
    }

    /* Feedback linearization */
    duty = 1 - 1 / m->v_dc_out * (m->v_dc_in - L / m->v_dc_in * v_lin);

    o->u = duty;

    if( o->u > 1.0f ) o->u = 1.0f;
    else if ( o->u < 0.0f ) o->u = 0.0f;

    return sizeof(boostConfigControl_t);
}
//-----------------------------------------------------------------------------
void boostControlEnergyMpcReset(void){

    uint32_t i;

    first_enter_lin = 0;
    first_enter_mpc = 0;

    for(i = 0; i < DMPC_CONFIG_NXM; i++) xm_1[i] = 0.0f;
    for(i = 0; i < (DMPC_CONFIG_NU + DMPC_CONFIG_ND); i++) u[i] = 0.0f;

    div_count = 0.0f;
}
//-----------------------------------------------------------------------------
void boostControlEnergyMpcExit(void){

    boostHwSetAdcScaledInterruptEnable(0);
}
//-----------------------------------------------------------------------------
void boostControlEnergyMpcOpt(void){

    uint32_t i;

    if( first_enter_mpc == 0 ){
        first_enter_mpc = 1;
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
    dmpcOpt(xm, xm_1, ref, u, &n_iters, du, 0);
    n_iters_float = (float) n_iters;

    /* Computes u = du + u_1 */
    sumv(u, du, DMPC_CONFIG_NU, u);

    /* Saves variables */
    for(i = 0; i < DMPC_CONFIG_NXM; i++){
        xm_1[i] = xm[i];
    }
    // -----------------------------------

    //v = k1 * y_r - k1 * y - k2 * y_dot;

    v = u[0] * V_GAIN;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void boostControlEnergyMpcEnableScaledInt(void){

    boostHwSetAdcScaledInterruptFactor(SW_DIV_FACTOR - 1);
    boostHwSetAdcScaledInterruptEnable(1);
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
