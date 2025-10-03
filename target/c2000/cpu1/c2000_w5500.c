//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "c2000_w5500.h"

#include "driverlib.h"
#include "device.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
/* 
 * Bytes swapped for all fields. Example:
 *
 * - IP
 *   - dec: 192, 168, 0, 100
 *   - hex: 0xC0, 0xA8, 0x00, 0x64
 *   - .ip: 0xA8C0, 0x6400
 *
 */
wiz_NetInfo gWIZNETINFO = { .mac = {0x0800, 0x00DC, 0xCDAB},
                            .ip = {0xF683, 0xAB4B}, //131, 246, 75, 171
                            .sn = {0xFFFF, 0x00FF},
                            .gw = {0xF683, 0xFE4B}, //131, 246, 75, 254
                            .dns = {0, 0},
                            .dhcp = NETINFO_DHCP };
//=============================================================================

//=============================================================================
/*------------------------------- Prototypes --------------------------------*/
//=============================================================================
static void C2000W5500SpiInit(void);
static void C2000W5500SpiPinMuxInit(void);

static void C2000W5500SpiWrite(uint16_t data);
static void C2000W5500SpiBurstWrite(uint16_t *buffer, uint16_t size);
static uint16_t C2000W5500SpiRead(void);
static void C2000W5500SpiBurstRead(uint16_t *buffer, uint16_t size);
static void C2000W5500SpiChipSelect(void);
static void C2000W5500SpiChipDeselect(void);

static void C2000W5500CriticalSectionEnter(void);
static void C2000W5500CriticalSectionExit(void);

static void C2000W5500PeripheralInit(void);

static void C2000W5500Reset(void);
static void C2000W5500DhcpInit(void);
static void C2000W5500IPAssignCB(void);
static void C2000W5500IPConflictCB(void);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void C2000W5500Init(void){

    C2000W5500PeripheralInit();

    C2000W5500Reset();

    reg_wizchip_cris_cbfunc(C2000W5500CriticalSectionEnter, C2000W5500CriticalSectionExit);
    reg_wizchip_cs_cbfunc(C2000W5500SpiChipSelect, C2000W5500SpiChipDeselect);
    reg_wizchip_spi_cbfunc(C2000W5500SpiRead, C2000W5500SpiWrite);
    reg_wizchip_spiburst_cbfunc(C2000W5500SpiBurstRead, C2000W5500SpiBurstWrite);

    wizchip_init(0, 0);

    while(wizphy_getphylink() == PHY_LINK_OFF) DEVICE_DELAY_US(1000UL * 1000UL);

    setSHAR(gWIZNETINFO.mac);
    /* Network initialization */
    ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);      // apply from dhcp

    //C2000W5500W5500DhcpInit();
}

//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
static void C2000W5500SpiInit(void){

    EALLOW;

    C2000W5500SpiPinMuxInit();

    SPI_disableModule(C2000_W5500_CONFIG_SPI);
    SPI_setConfig(
        C2000_W5500_CONFIG_SPI, DEVICE_LSPCLK_FREQ, SPI_PROT_POL1PHA0,
        SPI_MODE_MASTER, 
        10000000, 8
    );
    SPI_disableFIFO(C2000_W5500_CONFIG_SPI);
    SPI_disableLoopback(C2000_W5500_CONFIG_SPI);
    SPI_setEmulationMode(C2000_W5500_CONFIG_SPI, SPI_EMULATION_FREE_RUN);
    SPI_enableModule(C2000_W5500_CONFIG_SPI);

    EDIS;
}
//-----------------------------------------------------------------------------
static void C2000W5500PeripheralInit(void){

    C2000W5500SpiInit();
}
//-----------------------------------------------------------------------------
static void C2000W5500SpiPinMuxInit(void){

    GPIO_setPinConfig(C2000_W5500_CONFIG_SPI_GPIO_SIMO_CFG);
    GPIO_setPinConfig(C2000_W5500_CONFIG_SPI_GPIO_SOMI_CFG);
    GPIO_setPinConfig(C2000_W5500_CONFIG_SPI_GPIO_CLK_CFG);

    GPIO_setPinConfig(C2000_W5500_CONFIG_SPI_GPIO_CS_CFG);
    GPIO_setPadConfig(C2000_W5500_CONFIG_SPI_GPIO_CS_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(C2000_W5500_CONFIG_SPI_GPIO_CS_PIN, GPIO_DIR_MODE_OUT);
    GPIO_writePin(C2000_W5500_CONFIG_SPI_GPIO_CS_PIN, 1);

    GPIO_setPinConfig(C2000_W5500_CONFIG_SPI_GPIO_RST_CFG);
    GPIO_setPadConfig(C2000_W5500_CONFIG_SPI_GPIO_RST_PIN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(C2000_W5500_CONFIG_SPI_GPIO_RST_PIN, GPIO_DIR_MODE_OUT);
    GPIO_writePin(C2000_W5500_CONFIG_SPI_GPIO_RST_PIN, 0);
}
//-----------------------------------------------------------------------------
static void C2000W5500SpiWrite(uint16_t data){

    SPI_writeDataBlockingNonFIFO(C2000_W5500_CONFIG_SPI, data << (16U - C2000_W5500_CONFIG_SPI_DATA_LENGTH));
    SPI_readDataBlockingNonFIFO(C2000_W5500_CONFIG_SPI);

    while( SPI_isBusy(C2000_W5500_CONFIG_SPI) != 0 );
}
//-----------------------------------------------------------------------------
static void C2000W5500SpiBurstWrite(uint16_t *buffer, uint16_t size){

    uint16_t data;
    uint16_t bytes_sent;

    if(size == 0) return;

    bytes_sent = 0;

    while( 1 ){
        data = (*buffer & 0xFF);
        C2000W5500SpiWrite(data);

        bytes_sent++;
        if( bytes_sent >= size ) break;

        data = (*buffer & 0xFF00) >> 8;
        C2000W5500SpiWrite(data);

        bytes_sent++;
        if( bytes_sent >= size ) break;

        buffer++;
    }
}
//-----------------------------------------------------------------------------
static uint16_t C2000W5500SpiRead(void){

    SPI_writeDataBlockingNonFIFO(C2000_W5500_CONFIG_SPI, ((uint16_t)0xFF) << (16U - C2000_W5500_CONFIG_SPI_DATA_LENGTH));

    while( SPI_isBusy(C2000_W5500_CONFIG_SPI) != 0 );

    return SPI_readDataBlockingNonFIFO(C2000_W5500_CONFIG_SPI);
}
//-----------------------------------------------------------------------------
static void C2000W5500SpiBurstRead(uint16_t *buffer, uint16_t size){

    uint16_t data;
    uint16_t bytes_read;

    if(size == 0) return;

    bytes_read = 0;

    while( 1 ){

        data = C2000W5500SpiRead();
        *buffer = data & 0x00FF;

        bytes_read++;
        if( bytes_read >= size ) break;

        data = C2000W5500SpiRead();
        *buffer = *buffer | ((data << 8 ) & 0xFF00);

        bytes_read++;
        if( bytes_read >= size ) break;

        buffer++;
    }
}
//-----------------------------------------------------------------------------
static void C2000W5500SpiChipSelect(void){

    GPIO_writePin(C2000_W5500_CONFIG_SPI_GPIO_CS_PIN, 0);
}
//-----------------------------------------------------------------------------
static void C2000W5500SpiChipDeselect(void){

    GPIO_writePin(C2000_W5500_CONFIG_SPI_GPIO_CS_PIN, 1);
}

//-----------------------------------------------------------------------------
static void C2000W5500CriticalSectionEnter(void){

}
//-----------------------------------------------------------------------------
static void C2000W5500CriticalSectionExit(void){

}
//-----------------------------------------------------------------------------
static void C2000W5500DhcpInit(void){

    uint32_t dhcpStatus;
    uint16_t buf[512];

    // must be set the default mac before DHCP started.
    setSHAR(gWIZNETINFO.mac);

    DHCP_init(0, buf);

    reg_dhcp_cbfunc(C2000W5500IPAssignCB, C2000W5500IPAssignCB, C2000W5500IPConflictCB);

    while(1){

        dhcpStatus = DHCP_run();

        if(dhcpStatus == DHCP_IP_LEASED) break;

        else if(dhcpStatus == DHCP_FAILED){
            DHCP_stop();
            ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
        }

        DEVICE_DELAY_US(1000UL * 500UL);
    }
}
//-----------------------------------------------------------------------------
static void C2000W5500IPAssignCB(void){

    getIPfromDHCP(gWIZNETINFO.ip);
    getGWfromDHCP(gWIZNETINFO.gw);
    getSNfromDHCP(gWIZNETINFO.sn);
    getDNSfromDHCP(gWIZNETINFO.dns);
    gWIZNETINFO.dhcp = NETINFO_DHCP;

    /* Network initialization */
    ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);      // apply from dhcp
}
//-----------------------------------------------------------------------------
static void C2000W5500IPConflictCB(void){
#ifdef C2000W5500_DEBUG
    printf("IP conflict.\r\n");
    while(1);
#endif
}
//-----------------------------------------------------------------------------
static void C2000W5500Reset(void){

    GPIO_writePin(C2000_W5500_CONFIG_SPI_GPIO_RST_PIN, 0);
    DEVICE_DELAY_US(1000UL * 500UL);
    GPIO_writePin(C2000_W5500_CONFIG_SPI_GPIO_RST_PIN, 1);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//=============================================================================
