/*
 * @file ocpZynqCpu0.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpZynqCpu0.h"

/* Open controller project */
#include "ocp/ocpTrace.h"
#include "ocp/ocpCS.h"
#include "ocp/ocpIf.h"

/* Inter-processor communication */
#include "ipc/ipcClient.h"

/* Controller lib */
#include "controller/controller.h"

/* Zynq-specific stuff */
#include "ipcClientZynq.h"

#include "zynqConfig.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeIpc(void *intcInst);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeTraces(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeControlSystem(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeInterface(void);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_ADDR        ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_ADR
#define OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_SIZE        ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_SIZE

#define OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_ADDR        ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_ADR
#define OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_SIZE        ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_SIZE
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpZynqCpu0Initialize(void *intcInst){

    ocpZynqCpu0InitializeIpc(intcInst);
    ocpZynqCpu0InitializeTraces();
    ocpZynqCpu0InitializeControlSystem();
    ocpZynqCpu0InitializeInterface();
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeIpc(void *intcInst){

    ipcClientZynqInitialize(intcInst);

    ipcClientInitialize(
        ipcClientZynqIrqSend, ipcClientZynqIrqReceive,
        ipcClientZynqLock, ipcClientZynqUnlock,
        OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_ADDR, OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_SIZE,
        OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_ADDR, OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_SIZE
    );

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeTraces(void){

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeControlSystem(void){

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeInterface(void){

    ocpIfInitialize();

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
