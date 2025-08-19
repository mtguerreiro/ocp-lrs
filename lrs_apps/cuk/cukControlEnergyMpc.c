/*
 * cukControlEnergy.c
 *
 *  Created on: 11.09.2023
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukControlEnergyMpc.h"

#include "ocpConfig.h"
#include "ocp/ocpTrace.h"

#include "cukConfig.h"

#include "mvops.h"
#include "dmpc.h"
#include "dmpc_defs.h"
#include "dmpc_matrices.h"

#include "controller/controller.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define V_GAIN          ((float) 1e6 )

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float p_in = 0.0f, p_out = 0.0f, y_dot = 0.0f, y = 0.0f, y_r = 0.0f;

static float xm[DMPC_CONFIG_NXM] = {0};
static float xm_1[DMPC_CONFIG_NXM] = {0};

static float ref[DMPC_CONFIG_NY];

static float u[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0};
static float du[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0};

static uint32_t first_enter = 0;

static float u_init, u_min, u_max;
static float x_init, x_min, x_max;

static float il_max = 7.0f;
static float il_min = 0.0f;

static float duty = 0.0f;

static float du_min_1, du_min_2;
static float du_max_1, du_max_2;

static float Kx[2] = {9.40077310e+01, 1.36080265e-02};
static float Ky = 3.79121039;

static float Co = CUK_CONFIG_C_O;

static float du_1[DMPC_CONFIG_NC];
static float aux[DMPC_CONFIG_NC_x_NU];

static float freq_en = 0.0f;
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

    float **p;
    cukConfigMeasurements_t *hwm;
    cukConfigSwMeasurements_t *swm;
    cukConfigReferences_t *r = (cukConfigReferences_t *)refs;
    cukConfigControl_t *o = (cukConfigControl_t *)outputs;

    static float x1, x2, x3, x4;
    static float e_x1, e_x2, e_x3, e_x4;
    static float x1_r, x2_r, x3_r, x4_r;
    static float e_x1_r, e_x2_r, e_x3_r, e_x4_r;
    static float v;
    uint32_t i;

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
    e_x4 = (0.5f) * Co * x4 * x4;
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
    e_x4_r = (0.5f) * Co * x4_r * x4_r;
    y_r = e_x1_r + e_x2_r + e_x3_r + e_x4_r;

    /* Initialization */
    if( first_enter == 0 ){
        first_enter = 1;

        u[0] = 0;
        xm_1[0] = y;
    }

    /* Updates bounds */
    u_min = hwm->vi_dc / CUK_CONFIG_L_IN * (hwm->vi_dc - x3) / V_GAIN;
    u_max = hwm->vi_dc * hwm->vi_dc / CUK_CONFIG_L_IN / V_GAIN;
    //DMPC_CONFIG_U_MIN[0] = u_min;
    //DMPC_CONFIG_U_MAX[0] = u_max;

    x_min = il_min * hwm->vi_dc - p_out;
    x_max = il_max * hwm->vi_dc - p_out;
    //DMPC_CONFIG_XM_MIN[0] = x_min;
    //DMPC_CONFIG_XM_MAX[0] = x_max;

    du_min_1 = (x_min - y_dot) / 10.0f - u[0];
    du_min_2 = u_min;

    du_max_1 = (x_max - y_dot) / 10.0f - u[0];
    du_max_2 = u_max;

    /* Assembles state vector */
    xm[0] = y;
    xm[1] = y_dot;

    /* Delay compensation */
    dmpcDelayComp(xm, xm, u);

    /* Optimization */
    //dmpcOpt(xm, xm_1, &y_r, u, 0, du, 0);
    du[0] = -Kx[0] * (xm[0] - xm_1[0]) - Kx[1] * (xm[1] - xm_1[1]) - Ky * (xm[0] - y_r);

    if( freq_en > 0.5f ){
        mulmv(DMPC_M_Fj_3, DMPC_CONFIG_NU, du_1, DMPC_CONFIG_NC_x_NU, aux);
        du[0] = du[0] - aux[0];
        for(i = 0; i < (DMPC_CONFIG_NC - 1); i++){
            du_1[i] = du_1[i+1];
        }
        du_1[DMPC_CONFIG_NC - 1] = du[0];
    }

    if( du[0] < du_min_1 ) du[0] = du_min_1;
    if( du[0] < du_min_2 ) du[0] = du_min_2;

    if( du[0] > du_max_1 ) du[0] = du_max_1;
    if( du[0] > du_max_2 ) du[0] = du_max_2;

    /* Computes u = du + u_1 */
    sumv(u, du, DMPC_CONFIG_NU, u);

    /* Saves variables */
    for(i = 0; i < DMPC_CONFIG_NXM; i++){
        xm_1[i] = xm[i];
    }

    v = u[0] * V_GAIN;

    duty =  1 - (hwm->vi_dc * hwm->vi_dc / CUK_CONFIG_L_IN - v) * CUK_CONFIG_L_IN / (x3 * hwm->vi_dc);

    o->u = duty;

    if( o->u > 1.0f ) o->u = 1.0f;
    else if ( o->u < 0.0f ) o->u = 0.0f;

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t cukControlEnergyMpcSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    Kx[0] = *p++;
    Kx[1] = *p++;

    Ky = *p++;

    il_max = *p++;
    il_min = *p++;

    Co = *p++;

    freq_en = *p++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlEnergyMpcGetParams(void *buffer, uint32_t size){

    float *p = (float *)buffer;

    *p++ = Kx[0];
    *p++ = Kx[1];

    *p++ = Ky;

    *p++ = il_max;
    *p++ = il_min;

    *p++ = Co;

    *p++ = freq_en;

    return 28;
}
//-----------------------------------------------------------------------------
void cukControlEnergyMpcReset(void){

    uint32_t i;

    first_enter = 0;

    for(i = 0; i < DMPC_CONFIG_NXM; i++) xm_1[i] = 0.0f;
    for(i = 0; i < (DMPC_CONFIG_NU + DMPC_CONFIG_ND); i++) u[i] = 0.0f;

    for(i = 0; i < DMPC_CONFIG_NC; i++) du_1[i] = 0.0f;

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

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
