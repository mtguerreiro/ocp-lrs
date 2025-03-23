/*
 * buckControlStartup.c
 *
 *  Created on: 26.10.2023
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckControlStartup.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include "buckConfig.h"

/* Controllers */
#include "controller.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float u = 0.0f;
static float ufinal = 0.5f;
static float uinc = 50.0f / 100000.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckControlStartupInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlStartupSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    uinc = *p++;
    ufinal = *p++;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlStartupGetParams(void *buffer, uint32_t size){

    float *p = (float *)buffer;

    *p++ = uinc;
    *p++ = ufinal;

    return 8;
}
//-----------------------------------------------------------------------------
int32_t buckControlStartupRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    buckConfigMeasurements_t *m = (buckConfigMeasurements_t *)meas;
    buckConfigReferences_t *r = (buckConfigReferences_t *)refs;
    buckConfigControl_t *o = (buckConfigControl_t *)outputs;

    u += uinc;

    if( u >= ufinal ) u = ufinal;
    if( u <= 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(buckConfigControl_t);
}
//-----------------------------------------------------------------------------
void buckControlStartupReset(void){

    u = 0.0f;
}
//-----------------------------------------------------------------------------
void buckControlStartupGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = buckControlStartupInitialize;
    cbs->run = buckControlStartupRun;
    cbs->setParams = buckControlStartupSetParams;
    cbs->getParams = buckControlStartupGetParams;
    cbs->reset = buckControlStartupReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
