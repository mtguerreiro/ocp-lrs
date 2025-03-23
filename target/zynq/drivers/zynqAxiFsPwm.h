
#ifndef ZYNQ_AXI_FS_PWM_H_
#define ZYNQ_AXI_FS_PWM_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define ZYNQ_AXI_FS_PWM_RESET_OFS                   0
#define ZYNQ_AXI_FS_PWM_RESET_MASK                  (1 << ZYNQ_AXI_FS_PWM_RESET_OFS)

#define ZYNQ_AXI_FS_PWM_OUTPUT_ENABLE_OFS           1
#define ZYNQ_AXI_FS_PWM_OUTPUT_ENABLE_MASK          (1 << ZYNQ_AXI_FS_PWM_OUTPUT_ENABLE_OFS)

#define ZYNQ_AXI_FS_PWM_OVF_TRIGGER_ENABLE_OFS      2
#define ZYNQ_AXI_FS_PWM_OVF_TRIGGER_ENABLE_MASK     (1 << ZYNQ_AXI_FS_PWM_OVF_TRIGGER_ENABLE_OFS)

#define ZYNQ_AXI_FS_PWM_INV_OFS                     3
#define ZYNQ_AXI_FS_PWM_INV_MASK                    (1 << ZYNQ_AXI_FS_PWM_INV_OFS)

#define ZYNQ_AXI_FS_PWM_MODE_OFS                    4
#define ZYNQ_AXI_FS_PWM_MODE_MASK                   (1 << ZYNQ_AXI_FS_PWM_MODE_OFS)

#define ZYNQ_AXI_FS_PWM_LS_SW_OFS                   5
#define ZYNQ_AXI_FS_PWM_LS_SW_MASK                  (1 << ZYNQ_AXI_FS_PWM_LS_SW_OFS)

#define ZYNQ_AXI_FS_PWM_HS_SW_OFS                   6
#define ZYNQ_AXI_FS_PWM_HS_SW_MASK                  (1 << ZYNQ_AXI_FS_PWM_HS_SW_OFS)
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void zynqAxiFsPwmResetWrite(uint32_t pwmBase, uint32_t reset);
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmResetRead(uint32_t pwmBase);
//-----------------------------------------------------------------------------
void zynqAxiFsPwmOutputEnableWrite(uint32_t pwmBase, uint32_t enable);
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmOutputEnableRead(uint32_t pwmBase);
//-----------------------------------------------------------------------------
void zynqAxiFsPwmOvfTriggerEnableWrite(uint32_t pwmBase, uint32_t enable);
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmOvfTriggerEnableRead(uint32_t pwmBase);
//-----------------------------------------------------------------------------
void zynqAxiFsPwmInvWrite(uint32_t pwmBase, uint32_t enable);
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmInvRead(uint32_t pwmBase);
//-----------------------------------------------------------------------------
void zynqAxiFsPwmModeWrite(uint32_t pwmBase, uint32_t mode);
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmModeRead(uint32_t pwmBase);
//-----------------------------------------------------------------------------
void zynqAxiFsPwmLsSwWrite(uint32_t pwmBase, uint32_t state);
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmLsSwRead(uint32_t pwmBase);
//-----------------------------------------------------------------------------
void zynqAxiFsPwmHsSwWrite(uint32_t pwmBase, uint32_t state);
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmHsSwRead(uint32_t pwmBase);
//-----------------------------------------------------------------------------
void zynqAxiFsPwmControlWrite(uint32_t pwmBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmControlRead(uint32_t pwmBase);
//-----------------------------------------------------------------------------
void zynqAxiFsPwmPeriodWrite(uint32_t pwmBase, uint32_t period);
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmPeriodRead(uint32_t pwmBase);
//-----------------------------------------------------------------------------
void zynqAxiFsPwmDutyWrite(uint32_t pwmBase, uint32_t duty);
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmDutyRead(uint32_t pwmBase);
//-----------------------------------------------------------------------------
void zynqAxiFsPwmDeadTimeWrite(uint32_t pwmBase, uint32_t deadtime);
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmDeadTimeRead(uint32_t pwmBase);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* ZYNQ_AXI_FS_PWM_H_ */
