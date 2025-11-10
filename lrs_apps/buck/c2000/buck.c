
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buck.h"

#include "driverlib.h"
#include "device.h"
#include "c2000Config.h"

#include "stdio.h"

/* OCP config */
#include "ocpConfig.h"

/* Open controller project */
#include "ocp/ocpTrace.h"
#include "ocp/ocpCS.h"
#include "ocp/ocpIf.h"
#include "ocp/ocpOpil.h"

/* App */
#include "buckConfig.h"
#include "buckHw.h"
#include "buckHwIf.h"
#include "buckController.h"
//============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t buckOcpTracesInit(void);
//-----------------------------------------------------------------------------
static int32_t buckOcpControlSystemInit(void);
//-----------------------------------------------------------------------------
void buckAdcIrq(void);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define BUCK_OCP_CONFIG_TRACE_NAME_LEN          192
#define BUCK_OCP_CONFIG_TRACE_MAX_SIGNALS       16

#define BUCK_OCP_CONFIG_INPUT_BUF_SIZE           16
#define BUCK_OCP_CONFIG_OUTPUT_BUF_SIZE          8
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static char trace0Names[BUCK_OCP_CONFIG_TRACE_NAME_LEN];
static size_t trace0Data[BUCK_OCP_CONFIG_TRACE_MAX_SIGNALS];

static float bInputs[BUCK_OCP_CONFIG_INPUT_BUF_SIZE];
static float bOutputs[BUCK_OCP_CONFIG_OUTPUT_BUF_SIZE];

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckInit(void){

    buckOcpTracesInit();

    buckHwInit(buckAdcIrq);
    buckHwIfInitialize();

    buckControllerInit();
    buckOcpControlSystemInit();

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t buckOcpTracesInit(void){

    ocpTraceConfig_t config;
    buckConfigMeasurements_t *meas;
    buckConfigControl_t *control;
    
    /* Initializes buck's trace */
    config.mem = (void *)C2000_CONFIG_MEM_TRACE_ADR;
    config.size = C2000_CONFIG_MEM_TRACE_SIZE_MAX / sizeof(float);
    config.data = (void **)trace0Data;
    config.names = trace0Names;
    
    ocpTraceInitialize(BUCK_CONFIG_OCP_TRACE_ID, &config, "Buck trace");
    
    /* Adds measurements and controls to trace */
    meas = (buckConfigMeasurements_t *)(bInputs);
    control = (buckConfigControl_t *)(bOutputs);
    ocpTraceAddSignal(BUCK_CONFIG_OCP_TRACE_ID, (void *)&meas->il, "Inductor current");
    ocpTraceAddSignal(BUCK_CONFIG_OCP_TRACE_ID, (void *)&meas->v_in, "Input voltage");
    ocpTraceAddSignal(BUCK_CONFIG_OCP_TRACE_ID, (void *)&meas->v_dc_in, "Input dc link voltage");
    ocpTraceAddSignal(BUCK_CONFIG_OCP_TRACE_ID, (void *)&meas->v_dc_out, "Output dc link voltage");
    ocpTraceAddSignal(BUCK_CONFIG_OCP_TRACE_ID, (void *)&meas->v_out, "Output voltage");
    ocpTraceAddSignal(BUCK_CONFIG_OCP_TRACE_ID, (void *)&control->u, "Duty cycle");

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckOcpControlSystemInit(void){

    ocpCSConfig_t config = {0};

    config.binputs = (void *)bInputs;
    config.boutputs = (void *)bOutputs;

    config.fhwInterface = buckHwIf;
    config.fhwStatus = buckHwStatus;
    config.fhwDisable = buckHwShutDown;
    config.fapplyOutputs = buckHwApplyOutputs;
    config.fgetInputs = buckHwGetMeasurements;

    config.frun = buckControllerRun;
    config.fcontrollerInterface = buckControllerIf;
    config.fcontrollerStatus = buckControllerStatus;

    config.fenable = buckHwEnable;
    config.fdisable = buckHwDisable;

    config.fonEntry = 0;
    config.fonExit = 0;

    ocpCSInitialize(BUCK_CONFIG_OCP_CS_ID, &config, "Buck controller");

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void buckAdcIrq(void){

    ocpCSRun(BUCK_CONFIG_OCP_CS_ID);
    ocpTraceSave(BUCK_CONFIG_OCP_TRACE_ID);
}
//-----------------------------------------------------------------------------
//=============================================================================
