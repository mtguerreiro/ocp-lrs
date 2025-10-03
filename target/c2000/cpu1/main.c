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
//=============================================================================

//=============================================================================
/*------------------------------- Prototypes --------------------------------*/
//=============================================================================
void mainInit(void);

void mainC2000Init(void);
void mainC2000InitCpu2(void);

int32_t mainTcpServerOcpIf(uint8_t sn, uint8_t* buf, uint32_t size, uint16_t port);

//=============================================================================

//=============================================================================
/*----------------------------------- Main ----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void main(void)
{
    uint8_t buffer[64];

    mainInit();

    while(1){
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
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void mainInit(void){

    mainC2000Init();

    ipcClientC2000Initialize(0);
    ipcClientInitialize(
        ipcClientC2000IrqSend, ipcClientC2000IrqReceive,
        ipcClientC2000Lock, ipcClientC2000Unlock,
        C2000_CONFIG_MEM_CPU2_TO_CPU1_ADR, C2000_CONFIG_MEM_CPU2_TO_CPU1_SIZE,
        C2000_CONFIG_MEM_CPU1_TO_CPU2_ADR, C2000_CONFIG_MEM_CPU1_TO_CPU2_SIZE
    );

    ocpIfInitialize();

    C2000W5500Init();

}
//-----------------------------------------------------------------------------
void mainC2000Init(void){

    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Initialize GPIO and configure the GPIO pin as a push-pull output
    //
    Device_initGPIO();
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT);

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    mainC2000InitCpu2();
}
//-----------------------------------------------------------------------------
void mainC2000InitCpu2(void){

    /* Initializes second LED for CPU2 */
    GPIO_setPadConfig(C2000_CONFIG_CPU2_LED, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(C2000_CONFIG_CPU2_LED, GPIO_DIR_MODE_OUT);
    GPIO_setMasterCore(C2000_CONFIG_CPU2_LED, GPIO_CORE_CPU2);
    
    /* Gives ownership of selected RAM sections to CPU2 */
    MemCfg_setGSRAMMasterSel(
        MEMCFG_SECT_GS7 | MEMCFG_SECT_GS8 | MEMCFG_SECT_GS9 | MEMCFG_SECT_GS10 | MEMCFG_SECT_GS11 | MEMCFG_SECT_GS12,
        MEMCFG_GSRAMMASTER_CPU2
    );

    /* Acks and clears CPU2->CPU1 data IPC flag */
    HWREG(IPC_BASE + IPC_O_ACK) = 1UL << C2000_CONFIG_CPU2_CPU1_FLAG;
    HWREG(IPC_BASE + IPC_O_CLR) = 1UL << C2000_CONFIG_CPU2_CPU1_FLAG;

    /*
     * Signals to CPU2 that CPU1 initialized already (all required peripheral
     * ownerships were given).
     */
    HWREG(IPC_BASE + IPC_O_SET) = 1UL << C2000_CONFIG_CPU1_INIT;

    /* Now, wait until CPU2 is ready */
    while( !(HWREG(IPC_BASE + IPC_O_STS) & (1UL << C2000_CONFIG_CPU2_INIT)) );

    /* Acks the IPC flag */
    HWREG(IPC_BASE + IPC_O_ACK) = 1UL << C2000_CONFIG_CPU2_INIT;
    HWREG(IPC_BASE + IPC_O_CLR) = 1UL << C2000_CONFIG_CPU2_INIT;
}
//-----------------------------------------------------------------------------
int32_t mainTcpServerOcpIf(uint8_t sn, uint8_t* buf, uint32_t size, uint16_t port){
    
   int32_t ret;
   uint8_t *p;
   uint32_t rxSize;
   uint32_t txSize;

#if TCP_SERVER_ECHO_CFG_DEBUG == 1
   uint8_t destip[4];
   uint16_t destport;
#endif

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)
         {
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
            getSn_DIPR(sn, destip);
            destport = getSn_DPORT(sn);

            printf("%s (sn %d): Connected - %d.%d.%d.%d : %d\r\n", __func__, sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
            setSn_IR(sn,Sn_IR_CON);
         }
         if((getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
         {
            ret = recv(sn, buf, 4);
            if(ret != 4)  return -1;

            memcpy((void *)&rxSize, (void *)buf, sizeof(rxSize));

            ret = recv(sn, buf, rxSize);
            if(ret != rxSize) return -1;

            p = buf;
            ret = ocpIf(buf, rxSize >> 1, (void **)&p, size);
            txSize = ret << 1;

            ret = send(sn, (uint8_t *)&txSize, 4);
            if( ret < 4 ){
                close(sn);
                return -1;
            }

            ret = send(sn, p, txSize);
            if( ret != txSize ){
                close(sn);
                return -1;                
            }
         }
         break;
      case SOCK_CLOSE_WAIT :
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
         printf("%s (sn %d): CloseWait\r\n", __func__, sn);
#endif
         if((ret = disconnect(sn)) != SOCK_OK) return ret;
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
         printf("%s (sn %d): Socket closed\r\n", __func__, sn);
#endif
         break;
      case SOCK_INIT :
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
         printf("%s (sn %d): Listen, TCP server loopback, port [%d]\r\n", __func__, sn, port);
#endif
         if( (ret = listen(sn)) != SOCK_OK) return ret;
         break;
      case SOCK_CLOSED:
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
         printf("%s (sn %d): TCP server loopback start\r\n", __func__, sn);
#endif
         if((ret = socket(sn, Sn_MR_TCP, port, SF_TCP_NODELAY)) != sn) return ret;
#if TCP_SERVER_ECHO_CFG_DEBUG == 1
         printf("%s (sn %d): Socket opened, port [%d]\r\n", __func__, sn, port);
#endif
         break;
      default:
         break;
   }
   
   return 1;
}
//-----------------------------------------------------------------------------
//=============================================================================
