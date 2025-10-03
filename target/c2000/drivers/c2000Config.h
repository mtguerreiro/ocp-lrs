
#ifndef C2000_CONFIG_H_
#define C2000_CONFIG_H_

#include "driverlib.h"
#include "device.h"

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
/* Server's port that will be listening for connections */
#define C2000_CONFIG_TCP_SERVER_SN_OCP          0
#define C2000_CONFIG_TCP_SERVER_PORT_OCP        8080

/* GPIOs */
#define C2000_CONFIG_CPU2_LED                   34U
#define C2000_CONFIG_CPU2_GPIO_0                8U
#define C2000_CONFIG_CPU2_GPIO_1                9U
#define C2000_CONFIG_CPU2_GPIO_2                25U

/* IPC flags */
#define C2000_CONFIG_CPU1_INIT                  16U
#define C2000_CONFIG_CPU2_INIT                  16U
#define C2000_CONFIG_CPU1_CPU2_FLAG             0U
#define C2000_CONFIG_CPU2_CPU1_FLAG             10U

/* CPU1 to CPU2 data */
#define C2000_CONFIG_MEM_CPU1_TO_CPU2_ADR       0x00012000 // MEMCFG_SECT_GS6
#define C2000_CONFIG_MEM_CPU1_TO_CPU2_SIZE      0x00001000

/* CPU2 to CPU1 data */
#define C2000_CONFIG_MEM_CPU2_TO_CPU1_ADR       0x00013000 // MEMCFG_SECT_GS7
#define C2000_CONFIG_MEM_CPU2_TO_CPU1_SIZE      0x00001000

/* RAM memory for scope data */
#define C2000_CONFIG_MEM_TRACE_ADR              0x00014000 // MEMCFG_SECT_GS8 - GS12
#define C2000_CONFIG_MEM_TRACE_SIZE_MAX         (0x001000 * 5)
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* C2000_CONFIG_H_ */
