
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlCpl.h"
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
    float v_min;
    float v_max;
}ctlparams_t;

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static ctlparams_t params;

static float e = 0.0f;

static float ev = 0.0f;
static float ev_1 = 0.0f;

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

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    if( (m->v_in > params.v_max) || (m->v_in < params.v_min) ){
        o->u = 0.0f;
        return -1;
    }

    ev = r->v_out - m->v_dc_out;
    e = e + (params.dt / 2.0f) * ( ev + ev_1 );
    ev_1 = ev;

    u = - params.ki * m->il - params.kv * m->v_dc_out - params.k_ev * e;

    if( u > 1.0f ) u = 1.0f;
    else if( u < 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlCplSetParams(void *buffer, uint32_t size){

    memcpy((void *)&params, buffer, size);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlCplGetParams(void *buffer, uint32_t size){

    memcpy(buffer, (void *)&params, sizeof(ctlparams_t));

    return sizeof(ctlparams_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlCplReset(void){

}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlCplFirstEntry(void *meas, int32_t nmeas,
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
int32_t fsbuckboostControlCplLastExit(void *meas, int32_t nmeas,
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
