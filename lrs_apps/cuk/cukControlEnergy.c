/*
 * cukControlEnergy.c
 *
 *  Created on: 22.11.2023
 *      Author: marco
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukControlEnergy.h"

#include "ocpConfig.h"
#include "ocp/ocpTrace.h"

#include "cukConfig.h"

#include "controller/controller.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void cukControlEnergyFilterReset(void);
static float cukControlEnergyFilterRun(float x);
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    float a0;
    float a1;
    float a2;

    float b1;
    float b2;

    float x_1;
    float x_2;

    float y;
    float y_1;
    float y_2;

}filter_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float u = 0.0f;
static float k1 = 22864929.498738717, k2 = 9333.3333286044, k3 = -33914344673.813805, dt = 1e-05;
static float vc = 0.0f;
static float p_in = 0.0f, p_out = 0.0f, y_dot = 0.0f, y = 0.0f, y_r = 0.0f;

static float y_e = 0.0f, y_e_1 = 0.0f;

static filter_t notch = {.a0 = 1.0f, .a1 = -1.8954516649246216f, .a2 = 0.9887202978134155f,
        .b1 = -1.3892015218734741, .b2 = 0.48247018456459045f,
        .x_1 = 0.0f, .x_2 = 0.0f, .y = 0.0f, .y_1 = 0.0f, .y_2 = 0.0f
};

static float notchEnable = 0.0f;

static uint32_t first_enter = 0;

static float C_out = CUK_CONFIG_C_O;

static float kd = 0.5f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukControlEnergyInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlEnergyRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    cukConfigMeasurements_t *m = (cukConfigMeasurements_t *)meas;
    cukConfigReferences_t *r = (cukConfigReferences_t *)refs;
    cukConfigControl_t *o = (cukConfigControl_t *)outputs;

    static float e_x1, e_x2, e_x3, e_x4;
    static float x1_r, x2_r, x3_r, x4_r;
    static float e_x1_r, e_x2_r, e_x3_r, e_x4_r;
    static float v;

    vc = m->v1 + (1.0f / CUK_CONFIG_TF_N2N1) * m->v2;

    /* Energies */
    e_x1 = (0.5f) * CUK_CONFIG_L_IN * m->i1 * m->i1;
    e_x2 = (0.5f) * CUK_CONFIG_L_OUT * m->i2 * m->i2;
    e_x3 = (0.5f) * CUK_CONFIG_C_C * vc * vc * CUK_CONFIG_TF_N2N1_SQ / (CUK_CONFIG_TF_N2N1_SQ + 1.0f);
    e_x4 = (0.5f) * C_out * m->vo_dc * m->vo_dc;
    y = e_x1 + e_x2 + e_x3 + e_x4;

    /* Input, output and converter power */
    p_in = m->pi;
    p_out = m->po;
    y_dot = p_in - p_out;

    /* References */
    x4_r = r->vo - kd * m->io_filt;
    x1_r = p_out / m->vi_dc;
    x2_r = p_out / x4_r;
    x3_r = m->vi_dc + x4_r * (1.0f / CUK_CONFIG_TF_N2N1);

    e_x1_r = (0.5f) * CUK_CONFIG_L_IN * x1_r * x1_r;
    e_x2_r = (0.5f) * CUK_CONFIG_L_OUT * x2_r * x2_r;
    e_x3_r = (0.5f) * CUK_CONFIG_C_C * x3_r * x3_r * CUK_CONFIG_TF_N2N1_SQ / (CUK_CONFIG_TF_N2N1_SQ + 1.0f);
    e_x4_r = (0.5f) * C_out * x4_r * x4_r;
    y_r = e_x1_r + e_x2_r + e_x3_r + e_x4_r;

    /* Integrator */
    if( first_enter == 0 ){
        first_enter = 1;
        y_e = (-k1 * y - k2 * y_dot) / k3;
    }
    else{
        y_e = y_e_1 + dt * (y_r - y);
    }
    y_e_1 = y_e;

    /* Control */
    v = - k1 * y - k2 * y_dot - k3 * y_e;

    if( notchEnable > 0.5f ) v = cukControlEnergyFilterRun(v);

    u =  1 - (m->vi_dc * m->vi_dc / CUK_CONFIG_L_IN - v) * CUK_CONFIG_L_IN / (vc * m->vi_dc);

    if( u > 1.0f ) u = 1.0f;
    if( u < 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t cukControlEnergySetParams(void *params, uint32_t n){

    float *p = (float *)params;

    k1 = *p++;
    k2 = *p++;
    k3 = *p++;
    dt = *p++;

    notch.a0 = *p++;
    notch.a1 = *p++;
    notch.a2 = *p++;

    notch.b1 = *p++;
    notch.b2 = *p++;

    notchEnable = *p++;

    C_out = *p++;

    kd = *p++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlEnergyGetParams(void *buffer, uint32_t size){

    float *p = (float *)buffer;

    *p++ = k1;
    *p++ = k2;
    *p++ = k3;
    *p++ = dt;

    *p++ = notch.a0;
    *p++ = notch.a1;
    *p++ = notch.a2;

    *p++ = notch.b1;
    *p++ = notch.b2;

    *p++ = notchEnable;

    *p++ = C_out;

    *p++ = kd;

    return 48;
}
//-----------------------------------------------------------------------------
void cukControlEnergyReset(void){

    cukControlEnergyFilterReset();

    y_e_1 = 0.0f;
    u = 0.0f;
}
//-----------------------------------------------------------------------------
void cukControlEnergyGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = cukControlEnergyInitialize;
    cbs->run = cukControlEnergyRun;
    cbs->setParams = cukControlEnergySetParams;
    cbs->getParams = cukControlEnergyGetParams;
    cbs->reset = cukControlEnergyReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void cukControlEnergyFilterReset(void){

    notch.x_1 = 0.0f;
    notch.x_2 = 0.0f;

    notch.y = 0.0f;
    notch.y_1 = 0.0f;
    notch.y_2 = 0.0f;

    first_enter = 0;
}
//-----------------------------------------------------------------------------
static float cukControlEnergyFilterRun(float x){

    notch.y = -notch.b1 * notch.y_1 - notch.b2 * notch.y_2 + \
            notch.a0 * x + notch.a1 * notch.x_1 + notch.a2 * notch.x_2;

    notch.y_2 = notch.y_1;
    notch.y_1 = notch.y;

    notch.x_2 = notch.x_1;
    notch.x_1 = x;

    return notch.y;
}
//-----------------------------------------------------------------------------
//=============================================================================
