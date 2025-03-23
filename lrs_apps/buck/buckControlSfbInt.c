
#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckControlSfbInt.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include "buckConfig.h"

/* Controllers */
#include "controller.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
static float k1 = 0.0f, k2 = 0.0f, ke = 0.0f;
static float dt = 0.0f;
static float x1 = 0.0f, x2 = 0.0f;
static float xe = 0.0f, xe_1 = 0.0f;
static float u = 0.0f;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckControlSfbIntInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlSfbIntRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    buckConfigMeasurements_t *m = (buckConfigMeasurements_t *)meas;
    buckConfigReferences_t *r = (buckConfigReferences_t *)refs;
    buckConfigControl_t *o = (buckConfigControl_t *)outputs;

    /* States */
    x1 = m->i_l;
    x2 = m->v_dc_out;

    /* Deviations */
    xe = r->v_o - m->v_dc_out;

    /* Integral */
    xe = xe_1 + dt * (r->v_o - m->v_out);
    xe_1 = xe;

    /* Control */
    u = -k1 * x1 - k2 * x2 - ke * xe;

    if( u > 1.0f ) u = 1.0f;
    if( u < 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(buckConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t buckControlSfbIntSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    k1 = *p++;
    k2 = *p++;
    ke = *p++;
    dt = *p++;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlSfbIntGetParams(void *buffer, uint32_t size){

    float *p = (float *)buffer;

    *p++ = k1;
    *p++ = k2;
    *p++ = ke;
    *p++ = dt;

    return 16;
}
//-----------------------------------------------------------------------------
void buckControlSfbIntReset(void){

    xe_1 = 0.0f;
}
//-----------------------------------------------------------------------------
void buckControlSfbIntGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = buckControlSfbIntInitialize;
    cbs->run = buckControlSfbIntRun;
    cbs->setParams = buckControlSfbIntSetParams;
    cbs->getParams = buckControlSfbIntGetParams;
    cbs->reset = buckControlSfbIntReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
