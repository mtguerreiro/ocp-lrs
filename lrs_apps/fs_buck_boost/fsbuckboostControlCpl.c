
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlCpl.h"
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
static float v_min = 15.0f;
static float v_max = 35.0f;

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
int32_t fsbuckboostControlCplInit(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlCplRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    i = m->il;
    v = m->v_out;
    v_ref = r->v_out;

    if( (m->v_in > v_max) || (m->v_in < v_min) ){
        o->u = 0.0f;
        return -1;
    }

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
int32_t fsbuckboostControlCplSetParams(void *params, uint32_t size){

    float *p = (float *)params;

    ki = *p++;
    kv = *p++;
    k_ev = *p++;
    dt = *p++;
    v_min = *p++;
    v_max = *p++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlCplGetParams(void *buffer, uint32_t size){

    float *p = (float *)buffer;

    *p++ = ki;
    *p++ = kv;
    *p++ = k_ev;
    *p++ = dt;
    *p++ = v_min;
    *p++ = v_max;

    return 24;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlCplReset(void){

    e = 0.0f;

    ev_1 = 0.0f;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlCplFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlCplLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlCplGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlCplInit;
    cbs->run = fsbuckboostControlCplRun;
    cbs->setParams = fsbuckboostControlCplSetParams;
    cbs->getParams = fsbuckboostControlCplGetParams;
    cbs->reset = fsbuckboostControlCplReset;
    cbs->firstEntry = fsbuckboostControlCplFirstEntry;
    cbs->lastExit = fsbuckboostControlCplLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
