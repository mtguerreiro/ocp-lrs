
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukControlIdle.h"
#include "cukConfig.h"

#include "ocpConfig.h"

/* Controllers */
#include "controller/controller.h"

#include "cukHw.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define FS_BUCK_BOOST_CONTROL_IDLE_DECREMENT    0.005
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukControlIdleInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlIdleRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    cukConfigMeasurements_t *m = (cukConfigMeasurements_t *)meas;
    cukConfigControl_t *o = (cukConfigControl_t *)outputs;

    o->u = 0;

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t cukControlIdleSetParams(void *params, uint32_t size){

	return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlIdleGetParams(void *buffer, uint32_t size){

    return 0;
}
//-----------------------------------------------------------------------------
void cukControlIdleReset(void){

}
//-----------------------------------------------------------------------------
int32_t cukControlIdleFirstEntry(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    cukHwControllerDisable();

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t cukControlIdleLastExit(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    cukHwControllerEnable();

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
void cukControlIdleGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = cukControlIdleInitialize;
    cbs->run = cukControlIdleRun;
    cbs->setParams = cukControlIdleSetParams;
    cbs->getParams = cukControlIdleGetParams;
    cbs->reset = cukControlIdleReset;
    cbs->firstEntry = cukControlIdleFirstEntry;
    cbs->lastExit = cukControlIdleLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
