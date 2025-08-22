
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlBuckSfb.h"
#include "fsbuckboostConfig.h"

#include "controller/controller.h"

#include "string.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    float ki;
    float kv;
    float k_ev;
    float dt;
}ctlparams_t;

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static ctlparams_t params = {
    .ki = 0.02015303075313568f,
    .kv = -0.07867603003978729f,
    .k_ev = -4.631767272949219f,
    .dt = 1.0f / 100000.0f,
};

static float e = 0.0f;

static float ev = 0.0f;
static float ev_1 = 0.0f;

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

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    i = m->il;
    v = m->v_out;

    ev = r->v_out - v;
    e = e + (params.dt / 2.0f) * ( ev + ev_1 );
    ev_1 = ev;

    u = - params.ki * i - params.kv * v - params.k_ev * e;

    if( u > 1.0f ) u = 1.0f;
    else if( u < 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBuckSfbSetParams(void *buffer, uint32_t size){

    if( size != sizeof(ctlparams_t) ) return -1;
    memcpy( (void *)&params, buffer, sizeof(ctlparams_t) );

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBuckSfbGetParams(void *buffer, uint32_t size){

    if( size < sizeof(ctlparams_t) ) return -1;

    memcpy(buffer, (void *)&params, sizeof(ctlparams_t));

    return sizeof(ctlparams_t);
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

    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;

    e = 1 / params.k_ev * (-params.ki * m->il - params.kv * m->v_dc_out - m->v_dc_out / m->v_in);
    ev_1 = 0.0f;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBuckSfbLastExit(void *meas, int32_t nmeas,
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
