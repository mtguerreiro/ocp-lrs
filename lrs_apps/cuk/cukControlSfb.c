/*
 * cukControlSfb.c
 *
 *  Created on: 24.11.2023
 *      Author: marco
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukControlSfb.h"

#include "ocpConfig.h"
#include "ocp/ocpTrace.h"

#include "cukConfig.h"

#include "controller/controller.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
static float k[4] = {0.0f};
static float ke = 0.0f;
static float dt = 0.0f;
static float xs[4] = {0.0f};
static float xe = 0.0f, xe_1 = 0.0f;
static float us = 0.0f, ue = 0.0f;

static uint32_t first_enter = 0;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukControlSfbInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlSfbRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    float **p;
    cukConfigMeasurements_t *hwm;
    cukConfigSwMeasurements_t *swm;
    cukConfigReferences_t *r = (cukConfigReferences_t *)refs;
    cukConfigControl_t *o = (cukConfigControl_t *)outputs;

    static float x1e, x2e, x3e, x4e;
    static float vc;
    static float u, ud;

    p = (float **)meas;

    hwm = (cukConfigMeasurements_t *)p[0];
    swm = (cukConfigSwMeasurements_t *)p[1];

    vc = hwm->v1 + (1.0f / CUK_CONFIG_TF_N2N1) * hwm->v2;

    /* Deviations */
    x1e = hwm->i1 - xs[0];
    x2e = hwm->i2 - xs[1];
    x3e = vc - xs[2];
    x4e = hwm->vo_dc - xs[3];

    /* Integral */
    if( first_enter == 0 ){
        first_enter = 1;
        xe = -(k[0] * x1e + k[1] * x2e + k[2] * x3e +  k[3] * x4e) / ke;
    }
    else{
        xe = xe_1 + dt * (r->vo - hwm->vo_dc);
    }
    xe_1 = xe;

    /* Control */
    ue = ke * xe;
    ud = k[0] * x1e + k[1] * x2e + k[2] * x3e + k[3] * x4e;
    u = us - ud - ue;

    if( u > 1.0f ) u = 1.0f;
    if( u < 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t cukControlSfbSetParams(void *buffer, uint32_t size){

    (void)size;
    float *p = (float *)buffer;

    k[0] = *p++;
    k[1] = *p++;
    k[2] = *p++;
    k[3] = *p++;
    ke = *p++;
    dt = *p++;

    xs[0] = *p++;
    xs[1] = *p++;
    xs[2] = *p++;
    xs[3] = *p++;

    us = *p++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlSfbGetParams(void *buffer, uint32_t size){

    (void)size;
    float *p = (float *)buffer;

    *p++ = k[0];
    *p++ = k[1];
    *p++ = k[2];
    *p++ = k[3];
    *p++ = ke;
    *p++ = dt;

    *p++ = xs[0];
    *p++ = xs[1];
    *p++ = xs[2];
    *p++ = xs[3];

    *p++ = us;

    return 44;
}
//-----------------------------------------------------------------------------
void cukControlSfbReset(void){

    xe_1 = 0.0f;

    first_enter = 0;
}
//-----------------------------------------------------------------------------
void cukControlSfbGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = cukControlSfbInitialize;
    cbs->run = cukControlSfbRun;
    cbs->setParams = cukControlSfbSetParams;
    cbs->getParams = cukControlSfbGetParams;
    cbs->reset = cukControlSfbReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

