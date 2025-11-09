

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cuk.h"

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
#include "cukHw.h"
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
static uint32_t traceRawData[CUK_CONFIG_TRACE_SIZE_BYTES / sizeof(uint32_t)];

static char traceNames[CUK_CONFIG_TRACE_0_NAME_LEN];
static size_t traceData[CUK_CONFIG_TRACE_0_MAX_SIGNALS];

static float bInputs[CUK_CONFIG_INPUT_BUF_SIZE];
static float bOutputs[CUK_CONFIG_OUTPUT_BUF_SIZE];

static float *bInputsPointer[2];

static float texec = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void cukInit(void *params){

    (void)params;

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
    config.size = (uint32_t)( CUK_CONFIG_TRACE_SIZE_BYTES / sizeof(uint32_t) );
    config.data = (void **)traceData;
    config.dataSize = CUK_CONFIG_TRACE_0_MAX_SIGNALS;
    config.names = traceNames;
    config.namesBufferSize = sizeof(traceNames);

    ocpTraceInitialize(CUK_CONFIG_TRACE_ID, &config, "Cuk trace");

    cukInitializeTraceSignals();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukInitializeTraceSignals(void){

    cukConfigSwMeasurements_t *softMeas;
    cukConfigMeasurements_t *meas;
    cukConfigControl_t *outputs;

    /* Adds measurements to trace */
    meas = (cukConfigMeasurements_t *)bInputs;
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &meas->ii, "Input current");
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &meas->i1, "Primary inductor current");
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &meas->vi, "Input voltage");
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &meas->vi_dc, "Input dc link");
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &meas->v1, "Primary coupling cap voltage");

    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &meas->io, "Output current");
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &meas->i2, "Secondary inductor current");
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &meas->vo_dc, "Output dc link");
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &meas->vo, "Output voltage");
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &meas->v2, "Secondary coupling cap voltage");

    /* Adds control signals to trace */
    outputs = (cukConfigControl_t *)bOutputs;
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &outputs->u, "Duty-cycle");
    //ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &outputs->sw_o, "Output switch");

    /* Other signals to add */
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &texec, "Exec. time");

    softMeas = (cukConfigSwMeasurements_t *) &bInputs[sizeof(cukConfigMeasurements_t) >> 2];

    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &softMeas->io_filt, "Io filt");
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &softMeas->pi, "Input power");
    ocpTraceAddSignal(CUK_CONFIG_TRACE_ID, &softMeas->po, "Output power");

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukInitializeControlSystem(void){

    ocpCSConfig_t config;

    /* Initializes controller and hardware interface libs */
    cukControllerInit();
    cukHwIfInitialize();

    bInputsPointer[0] = bInputs;
    bInputsPointer[1] = &bInputs[sizeof(cukConfigMeasurements_t) >> 2];

    /*
     * Initializes control sys lib. Here we give bInputsPointer as buffer to
     * the inputs, because we have measurements and software measurements.
     * The vector bInputsPointer is used by the hw and the controllers to
     * access the hardware and software measurements.
     */
    config.binputs = (void *)bInputsPointer;
    config.boutputs = (void *)bOutputs;

    config.fhwInterface = cukHwIf;
    config.fhwStatus = cukHwStatus;
    config.fhwDisable = cukHwShutDown;

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

    (void)callbackRef;

    ocpCSRun(CUK_CONFIG_CS_ID);
    ocpTraceSave(CUK_CONFIG_TRACE_ID);
}
//-----------------------------------------------------------------------------
//=============================================================================
