/*
 * buckSfbControl.c
 *
 *  Created on: 30 de ago. de 2023
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckControlDisabled.h"
#include "buckConfig.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

/* Controllers */
#include "controller.h"

#include "buckHw.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckControlDisabledInitialize(void){

	//ocpTraceAddSignal(OCP_TRACE_1, (void *)&i_ac, "Grid current");
    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlDisabledRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    buckConfigMeasurements_t *m = (buckConfigMeasurements_t *)meas;
    buckConfigControl_t *o = (buckConfigControl_t *)outputs;

    o->u = 0.0f;

    return sizeof(buckConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t buckControlDisabledSetParams(void *params, uint32_t size){

	return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlDisabledGetParams(void *buffer, uint32_t size){

    return 0;
}
//-----------------------------------------------------------------------------
void buckControlDisabledReset(void){

}
//-----------------------------------------------------------------------------
int32_t buckControlDisabledFirstEntry(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    buckHwControllerDisable();

    return sizeof(buckConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t buckControlDisabledLastExit(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    buckHwControllerEnable();

    return sizeof(buckConfigControl_t);
}
//-----------------------------------------------------------------------------
void buckControlDisabledGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = buckControlDisabledInitialize;
    cbs->run = buckControlDisabledRun;
    cbs->setParams = buckControlDisabledSetParams;
    cbs->getParams = buckControlDisabledGetParams;
    cbs->reset = buckControlDisabledReset;
    cbs->firstEntry = buckControlDisabledFirstEntry;
    cbs->lastExit = buckControlDisabledLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
