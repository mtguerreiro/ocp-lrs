
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckController.h"

#include "buckConfig.h"

/* OCP */
#include "ocpConfig.h"
#include "ocp/ocpTrace.h"
#include "rp/rp.h"

/* Controller lib */
#include "controller/controller.h"
#include "controller/controllerIf.h"

/* Controllers */
#include "buckControlIdle.h"
#include "buckControlRamp.h"
#include "buckControlSfb.h"
#include "buckControlPlecs.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
    BUCK_CONTROLLER_IDLE,
    BUCK_CONTROLLER_RAMP,
    BUCK_CONTROLLER_SFB,
    BUCK_CONTROLLER_PLECS,
    BUCK_CONTROLLER_END
}appControllersEnum_t;

typedef struct{
    buckConfigReferences_t refs;
    controller_t controller;
    controllerCallbacks_t cbs[BUCK_CONTROLLER_END];
}buckController_t;

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
buckController_t xbuckControler;
//============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckControllerInit(void){

    controllerConfig_t config;

    controllerGetCbs_t ctlGetCbs[BUCK_CONTROLLER_END] = {0};
    ctlGetCbs[BUCK_CONTROLLER_IDLE] = buckControlIdleGetCallbacks;
    ctlGetCbs[BUCK_CONTROLLER_RAMP] = buckControlRampGetCallbacks;
    ctlGetCbs[BUCK_CONTROLLER_SFB] = buckControlSfbGetCallbacks;
    ctlGetCbs[BUCK_CONTROLLER_PLECS] = buckControlPlecsGetCallbacks;

    config.refBuffer = (void *)&xbuckControler.refs;
    config.refSize = sizeof(xbuckControler.refs);
    
    config.getCbs = ctlGetCbs;
    config.cbsBuffer = xbuckControler.cbs;
    config.nControllers = BUCK_CONTROLLER_END;

    controllerInit(&xbuckControler.controller, &config);

    controllerIfInit();
    controllerIfRegister(&xbuckControler.controller, BUCK_CONFIG_OCP_CS_ID);

    ocpTraceAddSignal(
        BUCK_CONFIG_OCP_TRACE_ID,
        (void *)&xbuckControler.refs.v_out, "Voltage reference"
    );

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControllerRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs){
    
    int32_t status;

    status = controllerRun(
        &xbuckControler.controller,
        inputs, ninputs,
        outputs, nmaxoutputs);

    return status;
}
//-----------------------------------------------------------------------------
int32_t buckControllerIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    return controllerIf(in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
int32_t buckControllerStatus(void){

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
