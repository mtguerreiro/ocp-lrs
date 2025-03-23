
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckController.h"

#include "buckConfig.h"

/* OCP */
#include "ocpConfig.h"
#include "ocpTrace.h"
#include "rp.h"

/* Controller lib */
#include "controller.h"
#include "controllerIf.h"

/* Controllers */
#include "buckControlDisabled.h"
#include "buckControlStartup.h"
#include "buckControlSfbInt.h"
//#include "appControllerCascaded.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
    BUCK_CONTROLLER_DISABLED,
    BUCK_CONTROLLER_STARTUP,
    BUCK_CONTROLLER_SFB_INT,
    //BUCK_CONTROLLER_CASCADED,
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
    ctlGetCbs[BUCK_CONTROLLER_DISABLED] = buckControlDisabledGetCallbacks;
    ctlGetCbs[BUCK_CONTROLLER_STARTUP] = buckControlStartupGetCallbacks;
    ctlGetCbs[BUCK_CONTROLLER_SFB_INT] = buckControlSfbIntGetCallbacks;
    //ctlGetCbs[BUCK_CONTROLLER_CASCADED] = appControlCascadedGetCallbacks;

    config.refBuffer = (void *)&xbuckControler.refs;
    config.refSize = sizeof(xbuckControler.refs);
    
    config.getCbs = ctlGetCbs;
    config.cbsBuffer = xbuckControler.cbs;
    config.nControllers = BUCK_CONTROLLER_END;

    controllerInit(&xbuckControler.controller, &config);

    controllerIfInit();
    controllerIfRegister(&xbuckControler.controller, OCP_CS_2); //OCP_CS_2 should be BUCK_CS

    //OCP_TRACE_2 should be BUCK_TRACE
    ocpTraceAddSignal(OCP_TRACE_2, (void *)&xbuckControler.refs.v_o, "Voltage reference");

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
