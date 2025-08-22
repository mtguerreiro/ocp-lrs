
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukControlRamp.h"
#include "ocpConfig.h"

#include "cukConfig.h"

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
int32_t cukControlRampInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlRampRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)nmaxoutputs;

    cukConfigControl_t *o = (cukConfigControl_t *)outputs;

    if( o->u < params.u_ref  ){
        o->u = o->u + params.u_step;
        if(o->u > params.u_ref ) o->u = params.u_ref ;
    }

    else{
        o->u = o->u - params.u_step;
        if(o->u < params.u_ref ) o->u = params.u_ref ;
    }

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t cukControlRampSetParams(void *buffer, uint32_t size){

    if( size != sizeof(ctlparams_t) ) return -1;
    memcpy( (void *)&params, buffer, sizeof(ctlparams_t) );

    if( params.u_step < 0 ) params.u_step = 0.0f;

    if( params.u_ref  > 1.0f ) params.u_ref  = 1.0f;
    else if( params.u_ref  < 0.0f) params.u_ref  = 0.0f;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlRampGetParams(void *buffer, uint32_t size){

    if( size < sizeof(ctlparams_t) ) return -1;

    memcpy(buffer, (void *)&params, sizeof(ctlparams_t));

    return sizeof(ctlparams_t);
}
//-----------------------------------------------------------------------------
void cukControlRampReset(void){

}
//-----------------------------------------------------------------------------
void cukControlRampGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = cukControlRampInitialize;
    cbs->run = cukControlRampRun;
    cbs->setParams = cukControlRampSetParams;
    cbs->getParams = cukControlRampGetParams;
    cbs->reset = cukControlRampReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
