
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlBoostCascFblin.h"
#include "fsbuckboostConfig.h"

#include "fsbuckboost.h"
#include "controller/controller.h"

#include "utils/dfilt.h"

#include "string.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    float n;
}ctlparams_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static uint32_t k;
static uint32_t n;

static ctlparams_t params = {
    .n = 10.0f
};
//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinInit(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    if( n != 0 ){
        if( k == 0 )
            fsbuckboostTrigSwIrq();

        k++;
        if( k >= n ) k = 0;
    }

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostCascFblinRun2(
    void *meas, int32_t nmeas,
    void *refs, int32_t nrefs
){

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinSetParams(void *buffer, uint32_t size){

    if( size != sizeof(ctlparams_t) ) return -1;
    memcpy( (void *)&params, buffer, sizeof(ctlparams_t) );

    n = (uint32_t) params.n;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinGetParams(void *buffer, uint32_t size){

    if( size < sizeof(ctlparams_t) ) return -1;

    memcpy(buffer, (void *)&params, sizeof(ctlparams_t));

    return sizeof(ctlparams_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostCascFblinReset(void){

}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    n = (uint32_t)params.n;
    k = 0;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinLastExit(void *meas, int32_t nmeas,
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
void fsbuckboostControlBoostCascFblinGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlBoostCascFblinInit;
    cbs->run = fsbuckboostControlBoostCascFblinRun;
    cbs->run2 = fsbuckboostControlBoostCascFblinRun2;
    cbs->setParams = fsbuckboostControlBoostCascFblinSetParams;
    cbs->getParams = fsbuckboostControlBoostCascFblinGetParams;
    cbs->reset = fsbuckboostControlBoostCascFblinReset;
    cbs->firstEntry = fsbuckboostControlBoostCascFblinFirstEntry;
    cbs->lastExit = fsbuckboostControlBoostCascFblinLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
