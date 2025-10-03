
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "driverlib.h"
#include "device.h"

#include "c2000Config.h"

#include "ipc/ipcServer.h"
#include "ipcServerC2000.h"

#include "ocp/ocpIf.h"

#include "c2000/buck.h"
#include "controller/controllerIf.h"
//=============================================================================

//=============================================================================
/*------------------------------- Prototypes --------------------------------*/
//=============================================================================
static void mainInit(void);
static void mainInitC2000(void);
static void mainInitIpc(void);
//=============================================================================

//=============================================================================
/*----------------------------------- Main ----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void main(void)
{
    mainInit();

    while(1);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void mainInit(void){

    mainInitC2000();

    mainInitIpc();

    ocpIfInitialize();
    
    buckInit();
}
//-----------------------------------------------------------------------------
static void mainInitC2000(void){

    /*
     * Initialize device clock and peripherals. We don't initialize GPIOs
     * because this will be done by CPU1.
     */
    Device_init();

    /* Initialize PIE and clear PIE registers. Disables CPU interrupts */
    Interrupt_initModule();

    /*
     * Initialize the PIE vector table with pointers to the shell Interrupt
     * Service Routines (ISR).
     */
    Interrupt_initVectorTable();

    /* Waits until CPU1 has initialized */
    while( !(HWREG(IPC_BASE + IPC_O_STS) & (1UL << C2000_CONFIG_CPU1_INIT)) );

    /* Acks the IPC flag */
    HWREG(IPC_BASE + IPC_O_ACK) = 1UL << C2000_CONFIG_CPU1_INIT;
    HWREG(IPC_BASE + IPC_O_CLR) = 1UL << C2000_CONFIG_CPU1_INIT;

    /* Enable Global Interrupt (INTM) and realtime interrupt (DBGM) */
    EINT;
    ERTM;

    /* Signals CPU1 that we have initialized CPU2 */
    HWREG(IPC_BASE + IPC_O_SET) = 1UL << C2000_CONFIG_CPU2_INIT;
}
//-----------------------------------------------------------------------------
static void mainInitIpc(void){

    ipcServerC2000Initialize(0);
    ipcServerInitialize(
        ocpIf, ipcServerC2000IrqSend,
        C2000_CONFIG_MEM_CPU1_TO_CPU2_ADR, C2000_CONFIG_MEM_CPU1_TO_CPU2_SIZE,
        C2000_CONFIG_MEM_CPU2_TO_CPU1_ADR, C2000_CONFIG_MEM_CPU2_TO_CPU1_SIZE
    );
}
//-----------------------------------------------------------------------------
//=============================================================================
