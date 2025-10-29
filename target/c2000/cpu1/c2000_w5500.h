#ifndef C2000_W5500_H_
#define C2000_W5500_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================

#include "driverlib.h"
#include "device.h"

#include "wiznet/dhcp.h"
#include "wiznet/socket.h"
#include "wiznet/wizchip_conf.h"
#include "wiznet/loopback.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define C2000_W5500_CONFIG_SPI                      SPIA_BASE
#define C2000_W5500_CONFIG_SPI_DATA_LENGTH          8

#define C2000_W5500_CONFIG_SPI_GPIO_SIMO_PIN        58U
#define C2000_W5500_CONFIG_SPI_GPIO_SIMO_CFG        GPIO_58_SPISIMOA

#define C2000_W5500_CONFIG_SPI_GPIO_SOMI_PIN        59U
#define C2000_W5500_CONFIG_SPI_GPIO_SOMI_CFG        GPIO_59_SPISOMIA

#define C2000_W5500_CONFIG_SPI_GPIO_CLK_PIN         60U
#define C2000_W5500_CONFIG_SPI_GPIO_CLK_CFG         GPIO_60_SPICLKA

#define C2000_W5500_CONFIG_SPI_GPIO_CS_PIN          61U
#define C2000_W5500_CONFIG_SPI_GPIO_CS_CFG          GPIO_61_GPIO61

#define C2000_W5500_CONFIG_SPI_GPIO_RST_PIN         29U
#define C2000_W5500_CONFIG_SPI_GPIO_RST_CFG         GPIO_29_GPIO29
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
void C2000W5500Init(void);
//=============================================================================

#endif /* C2000_W5500_H_ */
