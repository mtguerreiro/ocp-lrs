
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "zynqAxiFsPwm.h"

#include "xil_io.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define ZYNQ_AXI_FS_PWM_CONTROL_REG_OFS	        0
#define ZYNQ_AXI_FS_PWM_PERIOD_REG_OFS	        4
#define ZYNQ_AXI_FS_PWM_DUTY_REG_OFS	        8
#define ZYNQ_AXI_FS_PWM_DEADTIME_REG_OFS	    12

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void zynqAxiFsPwmResetWrite(uint32_t pwmBase, uint32_t reset){

    uint32_t control;

    reset = ( reset & 0x01 ) << ZYNQ_AXI_FS_PWM_RESET_OFS;

    control = zynqAxiFsPwmControlRead(pwmBase) & (~ZYNQ_AXI_FS_PWM_RESET_MASK);

    control = control | reset;

    zynqAxiFsPwmControlWrite(pwmBase, control);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmResetRead(uint32_t pwmBase){

    uint32_t reset;

    reset = zynqAxiFsPwmControlRead(pwmBase) & (ZYNQ_AXI_FS_PWM_RESET_MASK);

    reset = reset >> ZYNQ_AXI_FS_PWM_RESET_OFS;

    return reset;
}
//-----------------------------------------------------------------------------
void zynqAxiFsPwmOutputEnableWrite(uint32_t pwmBase, uint32_t enable){

    uint32_t control;

    enable = (enable & 0x01) << ZYNQ_AXI_FS_PWM_OUTPUT_ENABLE_OFS;

    control = zynqAxiFsPwmControlRead(pwmBase) & (~ZYNQ_AXI_FS_PWM_OUTPUT_ENABLE_MASK);

    control = control | enable;

    zynqAxiFsPwmControlWrite(pwmBase, control);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmOutputEnableRead(uint32_t pwmBase){

    uint32_t enable;

    enable = zynqAxiFsPwmControlRead(pwmBase) & (ZYNQ_AXI_FS_PWM_OUTPUT_ENABLE_MASK);

    enable = enable >> ZYNQ_AXI_FS_PWM_OUTPUT_ENABLE_OFS;

    return enable;
}
//-----------------------------------------------------------------------------
void zynqAxiFsPwmOvfTriggerEnableWrite(uint32_t pwmBase, uint32_t enable){

    uint32_t control;

    enable = (enable & 0x01) << ZYNQ_AXI_FS_PWM_OVF_TRIGGER_ENABLE_OFS;

    control = zynqAxiFsPwmControlRead(pwmBase) & (~ZYNQ_AXI_FS_PWM_OVF_TRIGGER_ENABLE_MASK);

    control = control | enable;

    zynqAxiFsPwmControlWrite(pwmBase, control);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmOvfTriggerEnableRead(uint32_t pwmBase){

    uint32_t enable;

    enable = zynqAxiFsPwmControlRead(pwmBase) & (ZYNQ_AXI_FS_PWM_OVF_TRIGGER_ENABLE_MASK);

    enable = enable >> ZYNQ_AXI_FS_PWM_OVF_TRIGGER_ENABLE_OFS;

    return enable;
}
//-----------------------------------------------------------------------------
void zynqAxiFsPwmInvWrite(uint32_t pwmBase, uint32_t enable){

    uint32_t control;

    enable = (enable & 0x01) << ZYNQ_AXI_FS_PWM_INV_OFS;

    control = zynqAxiFsPwmControlRead(pwmBase) & (~ZYNQ_AXI_FS_PWM_INV_MASK);

    control = control | enable;

    zynqAxiFsPwmControlWrite(pwmBase, control);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmInvRead(uint32_t pwmBase){

    uint32_t enable;

    enable = zynqAxiFsPwmControlRead(pwmBase) & (ZYNQ_AXI_FS_PWM_INV_MASK);

    enable = enable >> ZYNQ_AXI_FS_PWM_INV_OFS;

    return enable;
}
//-----------------------------------------------------------------------------
void zynqAxiFsPwmModeWrite(uint32_t pwmBase, uint32_t mode){

    uint32_t control;

    mode = (mode & 0x01) << ZYNQ_AXI_FS_PWM_MODE_OFS;

    control = zynqAxiFsPwmControlRead(pwmBase) & (~ZYNQ_AXI_FS_PWM_MODE_MASK);

    control = control | mode;

    zynqAxiFsPwmControlWrite(pwmBase, control);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmModeRead(uint32_t pwmBase){
    
    uint32_t mode;

    mode = zynqAxiFsPwmControlRead(pwmBase) & (ZYNQ_AXI_FS_PWM_MODE_MASK);

    mode = mode >> ZYNQ_AXI_FS_PWM_MODE_OFS;

    return mode;
}
//-----------------------------------------------------------------------------
void zynqAxiFsPwmLsSwWrite(uint32_t pwmBase, uint32_t state){

    uint32_t control;

    state = (state & 0x01) << ZYNQ_AXI_FS_PWM_LS_SW_OFS;

    control = zynqAxiFsPwmControlRead(pwmBase) & (~ZYNQ_AXI_FS_PWM_LS_SW_MASK);

    control = control | state;

    zynqAxiFsPwmControlWrite(pwmBase, control);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmLsSwRead(uint32_t pwmBase){

    uint32_t state;

    state = zynqAxiFsPwmControlRead(pwmBase) & (ZYNQ_AXI_FS_PWM_LS_SW_MASK);

    state = state >> ZYNQ_AXI_FS_PWM_LS_SW_OFS;

    return state;
}
//-----------------------------------------------------------------------------
void zynqAxiFsPwmHsSwWrite(uint32_t pwmBase, uint32_t state){

    uint32_t control;

    state = (state & 0x01) << ZYNQ_AXI_FS_PWM_HS_SW_OFS;

    control = zynqAxiFsPwmControlRead(pwmBase) & (~ZYNQ_AXI_FS_PWM_HS_SW_MASK);

    control = control | state;

    zynqAxiFsPwmControlWrite(pwmBase, control);   
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmHsSwRead(uint32_t pwmBase){

    uint32_t state;

    state = zynqAxiFsPwmControlRead(pwmBase) & (ZYNQ_AXI_FS_PWM_HS_SW_MASK);

    state = state >> ZYNQ_AXI_FS_PWM_HS_SW_OFS;

    return state;
}
//-----------------------------------------------------------------------------
void zynqAxiFsPwmControlWrite(uint32_t pwmBase, uint32_t data){
    
    Xil_Out32(pwmBase + ZYNQ_AXI_FS_PWM_CONTROL_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmControlRead(uint32_t pwmBase){
    
    return Xil_In32(pwmBase + ZYNQ_AXI_FS_PWM_CONTROL_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiFsPwmPeriodWrite(uint32_t pwmBase, uint32_t period){
    
    Xil_Out32(pwmBase + ZYNQ_AXI_FS_PWM_PERIOD_REG_OFS, period);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmPeriodRead(uint32_t pwmBase){

    return Xil_In32(pwmBase + ZYNQ_AXI_FS_PWM_PERIOD_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiFsPwmDutyWrite(uint32_t pwmBase, uint32_t duty){
    
    Xil_Out32(pwmBase + ZYNQ_AXI_FS_PWM_DUTY_REG_OFS, duty);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmDutyRead(uint32_t pwmBase){

    return Xil_In32(pwmBase + ZYNQ_AXI_FS_PWM_DUTY_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiFsPwmDeadTimeWrite(uint32_t pwmBase, uint32_t deadtime){
    
    Xil_Out32(pwmBase + ZYNQ_AXI_FS_PWM_DEADTIME_REG_OFS, deadtime);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiFsPwmDeadTimeRead(uint32_t pwmBase){

    return Xil_In32(pwmBase + ZYNQ_AXI_FS_PWM_DEADTIME_REG_OFS);
}
//-----------------------------------------------------------------------------
//=============================================================================
