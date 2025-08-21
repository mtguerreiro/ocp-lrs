
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "uiface.h"

#include "zynqConfig.h"
/* Kernel */
#include "FreeRTOS.h"
#include "task.h"

/* Device and drivers */
#include "xparameters.h"
#include "netif/xadapter.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "xil_io.h"

/* Network settings */
#define UIFACE_CONFIG_USE_DHCP      ZYNQ_CONFIG_USE_DHCP

#if UIFACE_CONFIG_USE_DHCP==1
#include "lwip/dhcp.h"
#endif

/* lwip */
#include "lwip/sockets.h"
#include "lwipopts.h"

#include "ocp/ocpIf.h"

//#include "ctlrif/pynq/pil.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
#define UIFACE_CONFIG_SERVER_PORT_OCP               ZYNQ_CONFIG_TCP_SERVER_PORT_OCP
#define UIFACE_CONFIG_SERVER_PORT_OPIL              ZYNQ_CONFIG_TCP_SERVER_PORT_OPIL
#define UIFACE_CONFIG_THREAD_STACK_SIZE_DEFAULT     2048
#define UIFACE_CONFIG_THREAD_PRIO_DEFAULT           DEFAULT_THREAD_PRIO

/* Ticks to wait to process new request */
#define UIFACE_CONFIG_MUTEX_WAIT                    UIFACE_CONFIG_MUTEX_WAIT_MS / portTICK_PERIOD_MS

/* Ethernet settings */
#define UIFACE_PLAT_EMAC_BASEADDR                   XPAR_XEMACPS_0_BASEADDR

/* Zynq DNA */
#define UIFACE_DNA_BASEADDR                         XPAR_ZYNQ_AXI_DNA_0_BASEADDR

typedef struct{
    /* Server netif */
    struct netif servernetif;

    /* Mutex to ensure one request from the UI is processed at a time */
    SemaphoreHandle_t mutexocp;

    /* Mutex to ensure one request from the UI is processed at a time */
    SemaphoreHandle_t mutexopil;

}uifaceControl_t;
//=============================================================================


//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
uifaceControl_t xuifaceControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void uifocpIfThread(void);

/**
 * @brief Thread spawned for each connection.
 *
 * @param void *p Socket index.
 */
static void uifocpIfProcessThread(void *p);

/**
 * @brief Initializes socket and updates DHCP timer.
 */
static void uifaceNetworkThread(void *p);

/**
 * @brief Prints the specified IP4.
 * @param msg Message to print before printing the IP.
 * @param ip IP to be printed.
 */
static void uifacePrintIP(char *msg, ip_addr_t *ip);

/**
 * @brief Prints IP settings
 */
static void uifacePrintIPSettings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw);

#if LWIP_IPV6==0
#if UIFACE_CONFIG_USE_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif
#endif
void lwip_init();

//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void uiface(void *param){

    struct netif *netif;

#if UIFACE_CONFIG_USE_DHCP==1
    int mscnt = 0;
#endif

    xuifaceControl.mutexocp = xSemaphoreCreateMutex();
    xuifaceControl.mutexopil = xSemaphoreCreateMutex();

    /* initialize lwIP before calling sys_thread_new */
    lwip_init();

    /* Any thread using lwIP should be created using sys_thread_new */
    sys_thread_new(
        "uifNWThread", uifaceNetworkThread, NULL,
        UIFACE_CONFIG_THREAD_STACK_SIZE_DEFAULT,
        UIFACE_CONFIG_THREAD_PRIO_DEFAULT
    );

    netif = &xuifaceControl.servernetif;

#if UIFACE_CONFIG_USE_DHCP==1
    while (1) {

        vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);

        if (netif->ip_addr.addr) {
            xil_printf("DHCP request success\r\n");
            break;
        }

        mscnt += DHCP_FINE_TIMER_MSECS;

        if (mscnt >= DHCP_COARSE_TIMER_SECS * 2000) {
            xil_printf("ERROR: DHCP request timed out\r\n");
            xil_printf("Configuring default IP of 192.168.1.10\r\n");
            IP4_ADDR(&(netif->ip_addr),  192, 168, 1, 10);
            IP4_ADDR(&(netif->netmask), 255, 255, 255,  0);
            IP4_ADDR(&(netif->gw),  192, 168, 1, 1);
            break;
        }
    }
#endif

    uifacePrintIPSettings(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
    xil_printf("\r\n");

    sys_thread_new(
        "uifocpIfThread", (void(*)(void*))uifocpIfThread, 0,
        UIFACE_CONFIG_THREAD_STACK_SIZE_DEFAULT,
        UIFACE_CONFIG_THREAD_PRIO_DEFAULT
    );

    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void uifocpIfThread(void){

    int sock, new_sd;
    int size;

    struct sockaddr_in address, remote;

    memset(&address, 0, sizeof(address));

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return;

    address.sin_family = AF_INET;
    address.sin_port = htons(UIFACE_CONFIG_SERVER_PORT_OCP);
    address.sin_addr.s_addr = INADDR_ANY;


    if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
        return;

    lwip_listen(sock, 0);

    size = sizeof(remote);

    while (1) {
        if ((new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t *)&size)) > 0) {
            xil_printf("%s: received connection from", __FUNCTION__);
            uifacePrintIP(": ", (ip_addr_t *)&remote.sin_addr);

            sys_thread_new(
                "uifocpIfProcessThread", uifocpIfProcessThread,
                (void*)new_sd,
                UIFACE_CONFIG_THREAD_STACK_SIZE_DEFAULT,
                UIFACE_CONFIG_THREAD_PRIO_DEFAULT
            );
        }
    }
}
//-----------------------------------------------------------------------------
static void uifocpIfProcessThread(void *param){

    int sd = (int)param;
    char recvbuf[UIFACE_CONFIG_RECV_BUFFER];
    int32_t n;
    int32_t nrx;
    int32_t ret;
    int32_t size;


    uint8_t *p;

    /*
     * A mutex is added here so that we can ensure that only one request is
     * processed at a time. This is important to prevent writes to any shared
     * memory that may being read.
     */
    if( xSemaphoreTake( xuifaceControl.mutexocp, UIFACE_CONFIG_MUTEX_WAIT ) == pdFALSE ){
        /* Can't process the request, so closes the connection */
        xil_printf("%s: Couldn't take mutex, closing socket\r\n", __FUNCTION__);
        lwip_close(sd);
        vTaskDelete(NULL);
    }

    while (1) {

        /*
         * After receiving a connection, reads the first 4 bytes. They should
         * represent the number of bytes to come next.
         */
        nrx = 0;
        while( nrx < 4 ){
            n = lwip_read(sd, &recvbuf[nrx], 4 - nrx);
            if( n <= 0 ){
                xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
                break;
            }
            nrx += n;
        }
        if( nrx != 4 ){
            xil_printf("%s: error reading size from socket %d, closing socket\r\n", __FUNCTION__, sd);
            break;
        }
        size = *((uint32_t *)recvbuf);

        /* Read the number of expected bytes */
        nrx = 0;
        while( nrx < size ){
            n = lwip_read(sd, &recvbuf[nrx], size - nrx);
            if( n <= 0 ){
                xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
                break;
            }
            nrx += n;
        }
        if( nrx != size ){
            xil_printf("%s: error receiving all expected data from socket %d, closing socket\r\n", __FUNCTION__, sd);
            break;
        }

        /* Calls the interface */
        p = (uint8_t *)( recvbuf );
        ret = ocpIf((void *)p, size, (void **)(&p), UIFACE_CONFIG_RECV_BUFFER);

        /*
         * Now, sends the reply. The reply consists of the command status/size
         * (4 bytes), followed by data (if any).
         */
        // TODO: should we also do a while loop to send the data? Like in receiving?

        /* Writes back the command status */
        //*( (int32_t *)recvbuf ) = ret;
        n = lwip_write(sd, &ret, 4);
        if( n < 4 ){
            xil_printf("%s: error responding to client\'s request\r\n", __FUNCTION__);
            break;
        }

        /* Writes data */
        if( ret > 0 ){
            n = lwip_write(sd, p, ret);
            if( n < ret ) xil_printf("%s: error responding to client\'s request\r\n", __FUNCTION__);
        }
        break;
    }

    /* Closes connection */
    xSemaphoreGive( xuifaceControl.mutexocp );
    lwip_close(sd);
    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------
static void uifaceNetworkThread(void *p){

    struct netif *netif;
    uint32_t dnaLow, dnaHigh;

    /* The mac address of the board. this should be unique per board */
    unsigned char mac[6];
    ip_addr_t ipaddr, netmask, gw;

    dnaLow = Xil_In32(UIFACE_DNA_BASEADDR);
    dnaHigh = Xil_In32(UIFACE_DNA_BASEADDR + 4);
    mac[0] = (char)(  dnaHigh & 0xFF );
    mac[1] = (char)( (dnaHigh & 0xFF00) >> 8 );
    mac[2] = (char)( (dnaHigh & 0xFF0000) >> 16 );
    mac[3] = (char)( (dnaHigh & 0xFF000000) >> 24 );
    mac[4] = (char)(  dnaLow  & 0xFF );
    mac[5] = (char)( (dnaLow  & 0xFF00) >> 8 );
    xil_printf(
        "Board MAC: %X:%X:%X:%X:%X:%X\n",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
    );

#if UIFACE_CONFIG_USE_DHCP==1
    int mscnt = 0;
#endif

    netif = &xuifaceControl.servernetif;

    xil_printf("\r\n\r\n");
    xil_printf("----- LRS-SoC controller - initializing network settings ------\r\n");

#if UIFACE_CONFIG_USE_DHCP==0
    /* initialize IP addresses to be used */
    IP4_ADDR(&ipaddr,  131, 246, 75, 135);
    IP4_ADDR(&netmask, 255, 255, 255,  0);
    IP4_ADDR(&gw,      131, 246, 75, 254);
    uifacePrintIPSettings(&ipaddr, &netmask, &gw);
#endif

#if UIFACE_CONFIG_USE_DHCP==1
    ipaddr.addr = 0;
    gw.addr = 0;
    netmask.addr = 0;
#endif

    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac, UIFACE_PLAT_EMAC_BASEADDR)) {
        xil_printf("Error adding N/W interface\r\n");
        return;
    }

    netif_set_default(netif);

    /* specify that the network if is up */
    netif_set_up(netif);

    /* start packet receive thread - required for lwIP operation */
    sys_thread_new(
        "xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
        UIFACE_CONFIG_THREAD_STACK_SIZE_DEFAULT,
        UIFACE_CONFIG_THREAD_PRIO_DEFAULT
    );

#if UIFACE_CONFIG_USE_DHCP==1
    dhcp_start(netif);
    while (1) {
        vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
        dhcp_fine_tmr();
        mscnt += DHCP_FINE_TIMER_MSECS;
        if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
            dhcp_coarse_tmr();
            mscnt = 0;
        }
    }
#else
    vTaskDelete(NULL);
#endif

    return;
}
//-----------------------------------------------------------------------------
static void uifacePrintIP(char *msg, ip_addr_t *ip){

    xil_printf(msg);
    xil_printf(
        "%d.%d.%d.%d\n\r",
        ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip)
    );
}
//-----------------------------------------------------------------------------
static void uifacePrintIPSettings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw){

    uifacePrintIP("Board IP : ", ip);
    uifacePrintIP("Netmask  : ", mask);
    uifacePrintIP("Gateway  : ", gw);
}
//-----------------------------------------------------------------------------
//=============================================================================
