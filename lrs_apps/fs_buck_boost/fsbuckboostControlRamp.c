
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlRamp.h"

#include "ocpConfig.h"

#include "fsbuckboostConfig.h"

/* Controllers */
#include "controller/controller.h"

#include "string.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    float u_step;
    float u_ref;
}ctlparams_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static ctlparams_t params = {.u_ref = 0.5f, .u_step = 0.001f};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlRampInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlRampRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)nmaxoutputs;

    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;

    if( o->u < params.u_ref  ){
        o->u = o->u + params.u_step;
        if(o->u > params.u_ref ) o->u = params.u_ref ;
    }

    else{
        o->u = o->u - params.u_step;
        if(o->u < params.u_ref ) o->u = params.u_ref ;
    }

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlRampSetParams(void *buffer, uint32_t size){

    if( size != sizeof(ctlparams_t) ) return -1;
    memcpy( (void *)&params, buffer, sizeof(ctlparams_t) );

    if( params.u_step < 0 ) params.u_step = 0.0f;

    if( params.u_ref  > 1.0f ) params.u_ref  = 1.0f;
    else if( params.u_ref  < 0.0f) params.u_ref  = 0.0f;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlRampGetParams(void *buffer, uint32_t size){

    if( size < sizeof(ctlparams_t) ) return -1;

    memcpy(buffer, (void *)&params, sizeof(ctlparams_t));

    return sizeof(ctlparams_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlRampReset(void){

}
//-----------------------------------------------------------------------------
void fsbuckboostControlRampGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlRampInitialize;
    cbs->run = fsbuckboostControlRampRun;
    cbs->setParams = fsbuckboostControlRampSetParams;
    cbs->getParams = fsbuckboostControlRampGetParams;
    cbs->reset = fsbuckboostControlRampReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
