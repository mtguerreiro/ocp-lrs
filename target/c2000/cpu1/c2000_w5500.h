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
#define C2000_W5500_CONFIG_SPI                      SPIB_BASE
#define C2000_W5500_CONFIG_SPI_DATA_LENGTH          8

#define C2000_W5500_CONFIG_SPI_GPIO_SIMO_PIN        63U
#define C2000_W5500_CONFIG_SPI_GPIO_SIMO_CFG        GPIO_63_SPISIMOB

#define C2000_W5500_CONFIG_SPI_GPIO_SOMI_PIN        64U
#define C2000_W5500_CONFIG_SPI_GPIO_SOMI_CFG        GPIO_64_SPISOMIB

#define C2000_W5500_CONFIG_SPI_GPIO_CLK_PIN         65U
#define C2000_W5500_CONFIG_SPI_GPIO_CLK_CFG         GPIO_65_SPICLKB

#define C2000_W5500_CONFIG_SPI_GPIO_CS_PIN          66U
#define C2000_W5500_CONFIG_SPI_GPIO_CS_CFG          GPIO_66_GPIO66

#define C2000_W5500_CONFIG_SPI_GPIO_RST_PIN         27U
#define C2000_W5500_CONFIG_SPI_GPIO_RST_CFG         GPIO_27_GPIO27
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
void C2000W5500Init(void);
//=============================================================================

#endif /* C2000_W5500_H_ */
