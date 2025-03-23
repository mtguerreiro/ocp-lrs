

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukOpil.h"

/* Open controller project */
#include "ocpConfig.h"
#include "ocp/ocpTrace.h"
#include "ocp/ocpCS.h"
#include "ocp/ocpIf.h"
#include "ocp/ocpOpil.h"

/* Controller lib */
#include "controller/controller.h"

/* Application */
#include "cukConfig.h"

//#include "cukOpil.h"
#include "cukController.h"

#include "cukHwIf.h"
#include "cukHwOpil.h"
#include "cukConfig.h"

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t cukInitializeTrace(void);
//-----------------------------------------------------------------------------
static int32_t cukInitializeTraceSignals(void);
//-----------------------------------------------------------------------------
static int32_t cukInitializeControlSystem(void);
//-----------------------------------------------------------------------------
void cukAdcIrq(void *callbackRef);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static char traceRawData[50 * 1024 * 1024];

static char traceNames[CUK_CONFIG_TRACE_0_NAME_LEN];
static size_t traceData[CUK_CONFIG_TRACE_0_MAX_SIGNALS];

static float bInputs[CUK_CONFIG_INPUT_BUF_SIZE];
static float bOutputs[CUK_CONFIG_OUTPUT_BUG_SIZE];

static float texec = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void cukInit(void *intcInst){

    cukInitializeTrace();

    cukInitializeControlSystem();

    /* Initializes OPiL interface */
    ocpOpilConfig_t config;

    config.updateMeas = cukHwOpilUpdateMeasurements;
    config.updateSimData = cukHwOpilUpdateSimData;

    config.initControl = 0;
    config.runControl = cukAdcIrq;

    config.getControl = cukHwOpilGetControl;
    config.getControllerData = cukHwOpilGetControllerData;

    ocpOpilInitialize(&config);
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t cukInitializeTrace(void){

    ocpTraceConfig_t config;

    config.mem = (void *)traceRawData;
    config.size = sizeof(traceRawData);
    config.data = (void **)traceData;
    config.names = traceNames;

    ocpTraceInitialize(CUK_CONFIG_TRACE_ID, &config, "Cuk trace");

    cukInitializeTraceSignals();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukInitializeTraceSignals(void){

    cukConfigMeasurements_t *meas;
    cukConfigControl_t *outputs;

    /* Adds measurements to trace */
    meas = (cukConfigMeasurements_t *)bInputs;
    ocpTraceAddSignal(OCP_TRACE_1, &meas->ii, "Input current");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->i1, "Primary inductor current");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->vi, "Input voltage");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->vi_dc, "DC link voltage");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v1, "Primary coupling cap voltage");

    ocpTraceAddSignal(OCP_TRACE_1, &meas->io, "Output current");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->i2, "Secondary inductor current");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->vo, "Output voltage");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->vo_dc, "Output DC link voltage");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v2, "Secondary coupling cap voltage");

    /* Adds control signals to trace */
    outputs = (cukConfigControl_t *)bOutputs;
    ocpTraceAddSignal(OCP_TRACE_1, &outputs->u, "Duty-cycle");
    //ocpTraceAddSignal(OCP_TRACE_1, &outputs->sw_o, "Output switch");

    /* Other signals to add */
    ocpTraceAddSignal(OCP_TRACE_1, &texec, "Exec. time");

    ocpTraceAddSignal(OCP_TRACE_1, &meas->io_filt, "Io filt");

    ocpTraceAddSignal(OCP_TRACE_1, &meas->pi, "Input power");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->po, "Output power");

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukInitializeControlSystem(void){

    ocpCSConfig_t config;

    /* Initializes controller and hardware interface libs */
    cukControllerInit();
    cukHwIfInitialize();

    /* Initializes control sys lib */
    config.binputs = (void *)bInputs;
    config.boutputs = (void *)bOutputs;

    config.fhwInterface = cukHwIf;
    config.fhwStatus = cukHwStatus;

    //config.fgetInputs = buckOpilGetMeasurements;
    config.fgetInputs = cukHwGetMeasurements;

    //config.fapplyOutputs = buckOpilUpdateControl;
    config.fapplyOutputs = cukHwApplyOutputs;

    config.frun = cukControllerRun;
    config.fcontrollerInterface = cukControllerIf;
    config.fcontrollerStatus = cukControllerStatus;

    config.fenable = cukHwEnable;
    //config.fenable = 0;
    config.fdisable = cukHwDisable;
    //config.fdisable = buckOpilDisable;

    config.fonEntry = 0;
    config.fonExit = 0;

    ocpCSInitialize(CUK_CONFIG_CS_ID, &config, "Cuk control");

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void cukAdcIrq(void *callbackRef){

    ocpCSRun(CUK_CONFIG_CS_ID);
    ocpTraceSave(CUK_CONFIG_TRACE_ID);
}
//-----------------------------------------------------------------------------
//=============================================================================
