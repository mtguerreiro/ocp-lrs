//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "driverlib.h"
#include "device.h"

#include "c2000Config.h"
#include "c2000_w5500.h"

#include "ocp/ocpIf.h"
#include "ipc/ipcClient.h"
#include "ipcClientC2000.h"

#include "tcp_server.h"   
//=============================================================================

//=============================================================================
/*------------------------------- Prototypes --------------------------------*/
//=============================================================================
void mainInit(void);
void mainC2000Init(void);
void mainC2000InitCpu2(void);
//=============================================================================

//=============================================================================
/*----------------------------------- Main ----------------------------------*/
//=============================================================================
void main(void)
{
    uint8_t buffer[64];

    mainInit();

    while(1)
    {
        mainTcpServerOcpIf(
            C2000_CONFIG_TCP_SERVER_SN_OCP,
            buffer, sizeof(buffer),
            C2000_CONFIG_TCP_SERVER_PORT_OCP
        );
    }
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Initialization -------------------------------*/
//=============================================================================
void mainInit(void)
{
    mainC2000Init();

    // Initialize IPC between CPU1 and CPU2
    ipcClientC2000Initialize(0);
    ipcClientInitialize(
        ipcClientC2000IrqSend, ipcClientC2000IrqReceive,
        ipcClientC2000Lock, ipcClientC2000Unlock,
        C2000_CONFIG_MEM_CPU2_TO_CPU1_ADR, C2000_CONFIG_MEM_CPU2_TO_CPU1_SIZE,
        C2000_CONFIG_MEM_CPU1_TO_CPU2_ADR, C2000_CONFIG_MEM_CPU1_TO_CPU2_SIZE
    );

    // Initialize OCP communication layer
    ocpIfInitialize();

    // Initialize Ethernet (W5500)
    C2000W5500Init();
}
//-----------------------------------------------------------------------------

void mainC2000Init(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Initialize GPIO and configure LED1 as output
    //
    Device_initGPIO();
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);

    //
    // Initialize PIE module and vector table, disable CPU interrupts
    //
    Interrupt_initModule();
    Interrupt_initVectorTable();

    //
    // Enable global (INTM) and real-time (DBGM) interrupts
    //
    EINT;
    ERTM;

    //
    // Configure peripherals and memory ownership for CPU2
    //
    mainC2000InitCpu2();
}
//-----------------------------------------------------------------------------

//=============================================================================
// GPIO pin definitions for CPU2 peripherals
//=============================================================================
#define GPIO_EPWM2A_PIN        2U
#define GPIO_EPWM2B_PIN        3U
#define GPIO_EPWM4A_PIN        6U
#define GPIO_EPWM4B_PIN        7U
#define GPIO_RELAY1_PIN        8U
#define GPIO_RELAY2_PIN        9U
//=============================================================================

void mainC2000InitCpu2(void)
{
    //------------------------------------------------------------------------
    // (1) Example LED assigned to CPU2
    //------------------------------------------------------------------------
    GPIO_setPadConfig(C2000_CONFIG_CPU2_LED, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(C2000_CONFIG_CPU2_LED, GPIO_DIR_MODE_OUT);
    GPIO_setMasterCore(C2000_CONFIG_CPU2_LED, GPIO_CORE_CPU2);

    //------------------------------------------------------------------------
    // (2) Assign GS RAM blocks to CPU2 (must match linker file)
    //------------------------------------------------------------------------
    MemCfg_setGSRAMMasterSel(
        MEMCFG_SECT_GS7 | MEMCFG_SECT_GS8 | MEMCFG_SECT_GS9 |
        MEMCFG_SECT_GS10| MEMCFG_SECT_GS11| MEMCFG_SECT_GS12,
        MEMCFG_GSRAMMASTER_CPU2
    );

    //------------------------------------------------------------------------
    // (3) Assign peripherals to CPU2
    //------------------------------------------------------------------------
    SysCtl_selectCPUForPeripheral(SYSCTL_CPUSEL0_EPWM, 2U, SYSCTL_CPUSEL_CPU2);
    SysCtl_selectCPUForPeripheral(SYSCTL_CPUSEL0_EPWM, 4U, SYSCTL_CPUSEL_CPU2);
    SysCtl_selectCPUForPeripheral(SYSCTL_CPUSEL11_ADC, 1U, SYSCTL_CPUSEL_CPU2);
    SysCtl_selectCPUForPeripheral(SYSCTL_CPUSEL11_ADC, 2U, SYSCTL_CPUSEL_CPU2);
    SysCtl_selectCPUForPeripheral(SYSCTL_CPUSEL11_ADC, 3U, SYSCTL_CPUSEL_CPU2);

    //------------------------------------------------------------------------
    // (4) Configure EPWM2A/B GPIOs
    //------------------------------------------------------------------------
    GPIO_setPinConfig(GPIO_2_EPWM2A);
    GPIO_setPinConfig(GPIO_3_EPWM2B);
    GPIO_setPadConfig(GPIO_EPWM2A_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(GPIO_EPWM2B_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_EPWM2A_PIN, GPIO_QUAL_SYNC);
    GPIO_setQualificationMode(GPIO_EPWM2B_PIN, GPIO_QUAL_SYNC);
    GPIO_setMasterCore(GPIO_EPWM2A_PIN, GPIO_CORE_CPU2);
    GPIO_setMasterCore(GPIO_EPWM2B_PIN, GPIO_CORE_CPU2);

    //------------------------------------------------------------------------
    // (5) Configure EPWM4A/B GPIOs
    //------------------------------------------------------------------------
    GPIO_setPinConfig(GPIO_6_EPWM4A);
    GPIO_setPinConfig(GPIO_7_EPWM4B);
    GPIO_setPadConfig(GPIO_EPWM4A_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setPadConfig(GPIO_EPWM4B_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_EPWM4A_PIN, GPIO_QUAL_SYNC);
    GPIO_setQualificationMode(GPIO_EPWM4B_PIN, GPIO_QUAL_SYNC);
    GPIO_setMasterCore(GPIO_EPWM4A_PIN, GPIO_CORE_CPU2);
    GPIO_setMasterCore(GPIO_EPWM4B_PIN, GPIO_CORE_CPU2);

    //------------------------------------------------------------------------
    // (6) Configure relay GPIOs
    //------------------------------------------------------------------------
    GPIO_setPinConfig(GPIO_8_GPIO8);
    GPIO_setPadConfig(GPIO_RELAY1_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_RELAY1_PIN, GPIO_QUAL_SYNC);
    GPIO_setMasterCore(GPIO_RELAY1_PIN, GPIO_CORE_CPU2);

    GPIO_setPinConfig(GPIO_9_GPIO9);
    GPIO_setPadConfig(GPIO_RELAY2_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_RELAY2_PIN, GPIO_QUAL_SYNC);
    GPIO_setMasterCore(GPIO_RELAY2_PIN, GPIO_CORE_CPU2);

    //------------------------------------------------------------------------
    // (7) Enable/reset clocks for EPWM and ADC modules
    //------------------------------------------------------------------------
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM2);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM4);
    SysCtl_resetPeripheral(SYSCTL_PERIPH_RES_EPWM2);
    SysCtl_resetPeripheral(SYSCTL_PERIPH_RES_EPWM4);

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_ADCA);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_ADCB);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_ADCC);

    //------------------------------------------------------------------------
    // (8) TBCLK re-synchronization
    //------------------------------------------------------------------------
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_GTBCLKSYNC);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_GTBCLKSYNC);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //------------------------------------------------------------------------
    // (9) IPC handshake with CPU2
    //------------------------------------------------------------------------
    HWREG(IPC_BASE + IPC_O_ACK) = 1UL << C2000_CONFIG_CPU2_CPU1_FLAG;
    HWREG(IPC_BASE + IPC_O_CLR) = 1UL << C2000_CONFIG_CPU2_CPU1_FLAG;

    HWREG(IPC_BASE + IPC_O_SET) = 1UL << C2000_CONFIG_CPU1_INIT;

    while(!(HWREG(IPC_BASE + IPC_O_STS) & (1UL << C2000_CONFIG_CPU2_INIT))) { }

    HWREG(IPC_BASE + IPC_O_ACK) = 1UL << C2000_CONFIG_CPU2_INIT;
    HWREG(IPC_BASE + IPC_O_CLR) = 1UL << C2000_CONFIG_CPU2_INIT;
}
//=============================================================================
