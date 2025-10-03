
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buck.h"

#include "driverlib.h"
#include "device.h"

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
// #define BUCK_OCP_CONFIG_TRACE_0_NAME_LEN         800
// #define BUCK_OCP_CONFIG_TRACE_0_MAX_SIGNALS      20

#define BUCK_OCP_CONFIG_INPUT_BUF_SIZE           50
#define BUCK_OCP_CONFIG_OUTPUT_BUF_SIZE          20
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
// static char trace0Names[BUCK_OCP_CONFIG_TRACE_0_NAME_LEN];
// static size_t trace0Data[BUCK_OCP_CONFIG_TRACE_0_MAX_SIGNALS];
// static uint8_t traceBuffer[BUCK_CONFIG_OCP_TRACE_0_SIZE_BYTES];

static float bInputs[BUCK_OCP_CONFIG_INPUT_BUF_SIZE];
static float bOutputs[BUCK_OCP_CONFIG_OUTPUT_BUF_SIZE];

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckInit(void){

    // buckOcpTracesInit();

    buckHwInit(0);
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

    // ocpTraceConfig_t config;
    // stypesMeasurements_t *meas;
    // stypesControl_t *control;
    //
    // /* Initializes buck's trace */
    // config.mem = (void *)traceBuffer;
    // config.size = BUCK_CONFIG_OCP_TRACE_0_SIZE_BYTES;
    // config.data = (void **)trace0Data;
    // config.names = trace0Names;
    //
    // ocpTraceInitialize(BUCK_CONFIG_OCP_TRACE_ID, &config, "App trace");
    //
    // /* Adds measurements and controls to trace */
    // meas = (stypesMeasurements_t *)(bInputs);
    // control = (stypesControl_t *)(bOutputs);
    // ocpTraceAddSignal(BUCK_CONFIG_OCP_TRACE_ID, (void *)&meas->i, "Inductor current");
    // ocpTraceAddSignal(BUCK_CONFIG_OCP_TRACE_ID, (void *)&meas->v_out, "Output voltage");
    // ocpTraceAddSignal(BUCK_CONFIG_OCP_TRACE_ID, (void *)&control->u, "Duty-cycle");

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckOcpControlSystemInit(void){

    ocpCSConfig_t config = {0};

    config.binputs = (void *)bInputs;
    config.boutputs = (void *)bOutputs;

    config.fhwInterface = buckHwIf;
    config.fhwStatus = buckHwStatus;

    config.frun = buckControllerRun;
    config.fcontrollerInterface = buckControllerIf;
    config.fcontrollerStatus = buckControllerStatus;

    config.fenable = 0;
    config.fdisable = 0;

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

    // (void)callbackRef;
    //
    // ocpCSRun(BUCK_CONFIG_OCP_CS_ID);
    // ocpTraceSave(BUCK_CONFIG_OCP_TRACE_ID);
}
//-----------------------------------------------------------------------------
//=============================================================================
