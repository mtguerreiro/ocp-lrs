
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckControlIdle.h"
#include "buckConfig.h"

#include "ocpConfig.h"

/* Controllers */
#include "controller/controller.h"

#include "c2000/buckHw.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckControlIdleInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlIdleRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)nmaxoutputs;

    buckConfigControl_t *o = (buckConfigControl_t *)outputs;
    
    o->u = 0;

    return sizeof(buckConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t buckControlIdleSetParams(void *buffer, uint32_t size){

    (void)buffer;
    (void)size;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlIdleGetParams(void *buffer, uint32_t size){

    (void)buffer;
    (void)size;

    return 0;
}
//-----------------------------------------------------------------------------
void buckControlIdleReset(void){

}
//-----------------------------------------------------------------------------
int32_t buckControlIdleFirstEntry(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    buckHwControllerDisable();

    return sizeof(buckConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t buckControlIdleLastExit(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    buckHwControllerEnable();

    return sizeof(buckConfigControl_t);
}
//-----------------------------------------------------------------------------
void buckControlIdleGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = buckControlIdleInitialize;
    cbs->run = buckControlIdleRun;
    cbs->setParams = buckControlIdleSetParams;
    cbs->getParams = buckControlIdleGetParams;
    cbs->reset = buckControlIdleReset;
    cbs->firstEntry = buckControlIdleFirstEntry;
    cbs->lastExit = buckControlIdleLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
