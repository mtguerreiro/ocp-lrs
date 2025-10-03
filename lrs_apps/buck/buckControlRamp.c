
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckControlRamp.h"

#include "ocpConfig.h"

#include "buckConfig.h"

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
int32_t buckControlRampInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlRampRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)nmaxoutputs;

    buckConfigControl_t *o = (buckConfigControl_t *)outputs;

    if( o->u < params.u_ref  ){
        o->u = o->u + params.u_step;
        if(o->u > params.u_ref ) o->u = params.u_ref ;
    }

    else{
        o->u = o->u - params.u_step;
        if(o->u < params.u_ref ) o->u = params.u_ref ;
    }

    return sizeof(buckConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t buckControlRampSetParams(void *buffer, uint32_t size){

    if( size != sizeof(ctlparams_t) ) return -1;
    memcpy( (void *)&params, buffer, sizeof(ctlparams_t) );

    if( params.u_step < 0 ) params.u_step = 0.0f;

    if( params.u_ref  > 1.0f ) params.u_ref  = 1.0f;
    else if( params.u_ref  < 0.0f) params.u_ref  = 0.0f;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlRampGetParams(void *buffer, uint32_t size){

    if( size < sizeof(ctlparams_t) ) return -1;

    memcpy(buffer, (void *)&params, sizeof(ctlparams_t));

    return sizeof(ctlparams_t);
}
//-----------------------------------------------------------------------------
void buckControlRampReset(void){

}
//-----------------------------------------------------------------------------
void buckControlRampGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = buckControlRampInitialize;
    cbs->run = buckControlRampRun;
    cbs->setParams = buckControlRampSetParams;
    cbs->getParams = buckControlRampGetParams;
    cbs->reset = buckControlRampReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
