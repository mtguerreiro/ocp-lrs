
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlBuckSfb.h"
#include "fsbuckboostConfig.h"

#include "controller/controller.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================


//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float ki = 0.02015303075313568;
static float kv = -0.07867603003978729;
static float k_ev = -4.631767272949219;

static float e = 0.0f;

static float ev = 0.0f;
static float ev_1 = 0.0f;

static float v_ref = 6.0f;
static float dt = 1.0f / 100000.0f;

static float i;
static float v;
static float u;

//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBuckSfbInit(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBuckSfbRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    i = m->il;
    v = m->v_out;
    v_ref = r->v_out;

    ev = v_ref - v;
    e = e + (dt / 2.0f) * ( ev + ev_1 );
    ev_1 = ev;

    u = - ki * i - kv * v - k_ev * e;

    if( u > 1.0f ) u = 1.0f;
    else if( u < 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBuckSfbSetParams(void *params, uint32_t size){

    float *p = (float *)params;

    ki = *p++;
    kv = *p++;
    k_ev = *p++;
    dt = *p++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBuckSfbGetParams(void *buffer, uint32_t size){

    float *p = (float *)buffer;

    *p++ = ki;
    *p++ = kv;
    *p++ = k_ev;
    *p++ = dt;

    return 16;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBuckSfbReset(void){

    e = 0.0f;

    ev_1 = 0.0f;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBuckSfbFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBuckSfbLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBuckSfbGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlBuckSfbInit;
    cbs->run = fsbuckboostControlBuckSfbRun;
    cbs->setParams = fsbuckboostControlBuckSfbSetParams;
    cbs->getParams = fsbuckboostControlBuckSfbGetParams;
    cbs->reset = fsbuckboostControlBuckSfbReset;
    cbs->firstEntry = fsbuckboostControlBuckSfbFirstEntry;
    cbs->lastExit = fsbuckboostControlBuckSfbLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
