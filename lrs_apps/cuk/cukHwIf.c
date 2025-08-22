
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukHwIf.h"

#include "cukHw.h"

#include "cukConfig.h"

#include "rp/rp.h"

#include "string.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    rphandle_t handles[CUK_HW_IF_END];
    rpctx_t rp;
}cukHwIf_t;

typedef struct{

    cukHwIf_t interface;
}cukHwIfControl_t;

static cukHwIfControl_t hwControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t cukHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetOutputSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetOutputSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetFiltCoef(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetFiltCoef(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t cukHwIfInitialize(void){

    /* Initializes the request processor */
    rpInitialize(&hwControl.interface.rp, CUK_HW_IF_END, hwControl.interface.handles);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_RESET, cukHwIfSetPwmReset);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_RESET, cukHwIfGetPwmReset);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_OUTPUT_ENABLE, cukHwIfSetPwmOutputEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_OUTPUT_ENABLE, cukHwIfGetPwmOutputEnable);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_OVF_TRIGGER_ENABLE, cukHwIfSetPwmOvfTriggerEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_OVF_TRIGGER_ENABLE, cukHwIfGetPwmOvfTriggerEnable);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_FREQ, cukHwIfSetPwmFrequency);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_FREQ, cukHwIfGetPwmFrequency);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_DUTY, cukHwIfSetPwmDutyCycle);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_DUTY, cukHwIfGetPwmDutyCycle);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_DEAD_TIME, cukHwIfSetPwmDeadTime);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_DEAD_TIME, cukHwIfGetPwmDeadTime);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_ENABLE, cukHwIfSetAdcEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_ADC_ENABLE, cukHwIfGetAdcEnable);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_MANUAL_TRIG, cukHwIfSetAdcManualTrig);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_ADC_MANUAL_TRIG, cukHwIfGetAdcManualTrig);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_INT_ENABLE, cukHwIfSetAdcInterruptEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_ADC_INT_ENABLE, cukHwIfGetAdcInterruptEnable);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_SPI_FREQ, cukHwIfSetAdcSpiFreq);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_ADC_SPI_FREQ, cukHwIfGetAdcSpiFreq);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_LOAD_SWITCH, cukHwIfSetLoadSwitch);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_LOAD_SWITCH, cukHwIfGetLoadSwitch);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_OUTPUT_SWITCH, cukHwIfSetOutputSwitch);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_OUTPUT_SWITCH, cukHwIfGetOutputSwitch);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_MEAS_GAINS, cukHwIfSetMeasGains);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_MEAS_GAINS, cukHwIfGetMeasGains);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_CLEAR_STATUS, cukHwIfClearStatus);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_STATUS, cukHwIfGetStatus);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_FILT_COEF, cukHwIfSetFiltCoef);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_FILT_COEF, cukHwIfGetFiltCoef);
    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    int32_t status;

    status = rpRequest(&hwControl.interface.rp, in, insize, out, maxoutsize);

    return status;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t reset;

    if( insize != sizeof(reset) ) return -1;

    memcpy( (void *)&reset, in, sizeof(reset) );

    cukHwSetPwmReset(reset & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t reset;

    reset = cukHwGetPwmReset();

    if( maxoutsize < sizeof(reset) ) return -1;

    memcpy( *out, (void *)&reset, sizeof(reset) );

    return sizeof(reset);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t enable;

    if( insize != sizeof(enable) ) return -1;

    memcpy( (void *)&enable, in, sizeof(enable) );

    cukHwSetPwmOutputEnable(enable & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t enable;

    enable = cukHwGetPwmOutputEnable();

    if( maxoutsize < sizeof(enable) ) return -1;

    memcpy( *out, (void *)&enable, sizeof(enable) );

    return sizeof(enable);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t enable;

    if( insize != sizeof(enable) ) return -1;

    memcpy( (void *)&enable, in, sizeof(enable) );

    cukHwSetPwmOvfTriggerEnable(enable & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t enable;

    enable = cukHwGetPwmOvfTriggerEnable();

    if( maxoutsize < sizeof(enable) ) return -1;

    memcpy( *out, (void *)&enable, sizeof(enable) );

    return sizeof(enable);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t freq;

    if( insize != sizeof(freq) ) return -1;

    memcpy( (void *)&freq, in, sizeof(freq) );

    cukHwSetPwmFrequency(freq);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t freq;

    freq = cukHwGetPwmFrequency();

    if( maxoutsize < sizeof(freq) ) return -1;

    memcpy( *out, (void *)&freq, sizeof(freq) );

    return sizeof(freq);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    float duty;

    if( insize != sizeof(duty) ) return -1;

    memcpy( (void *)&duty, in, sizeof(duty) );

    cukHwSetPwmDuty(duty);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    float duty;

    duty = cukHwGetPwmDuty();

    if( maxoutsize < sizeof(duty) ) return -1;

    memcpy( *out, (void *)&duty, sizeof(duty) );

    return sizeof(duty);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    float deadtime;

    if( insize != sizeof(deadtime) ) return -1;

    memcpy( (void *)&deadtime, in, sizeof(deadtime) );

    cukHwSetPwmDeadTime(deadtime);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    float deadtime;

    deadtime = cukHwGetPwmDeadTime();

    if( maxoutsize < sizeof(deadtime) ) return -1;

    memcpy( *out, (void *)&deadtime, sizeof(deadtime) );

    return sizeof(deadtime);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t enable;

    if( insize != sizeof(enable) ) return -1;

    memcpy( (void *)&enable, in, sizeof(enable) );

    cukHwSetAdcEnable(enable & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t enable;

    enable = cukHwGetAdcEnable();

    if( maxoutsize < sizeof(enable) ) return -1;

    memcpy( *out, (void *)&enable, sizeof(enable) );

    return sizeof(enable);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t trigger;

    if( insize != sizeof(trigger) ) return -1;

    memcpy( (void *)&trigger, in, sizeof(trigger) );

    cukHwSetAdcManualTrigger(trigger & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t trigger;

    trigger = cukHwGetAdcManualTrigger();

    if( maxoutsize < sizeof(trigger) ) return -1;

    memcpy( *out, (void *)&trigger, sizeof(trigger) );

    return sizeof(trigger);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t enable;

    if( insize != sizeof(enable) ) return -1;

    memcpy( (void *)&enable, in, sizeof(enable) );

    cukHwSetAdcInterruptEnable(enable & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t enable;

    enable = cukHwGetAdcInterruptEnable();

    if( maxoutsize < sizeof(enable) ) return -1;

    memcpy( *out, (void *)&enable, sizeof(enable) );

    return sizeof(enable);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t freq;

    if( insize != sizeof(freq) ) return -1;

    memcpy( (void *)&freq, in, sizeof(freq) );

    cukHwSetAdcSpiFreq(freq);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t freq;

    freq = cukHwGetAdcSpiFreq();

    if( maxoutsize < sizeof(freq) ) return -1;

    memcpy( *out, (void *)&freq, sizeof(freq) );

    return sizeof(freq);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t state;

    if( insize != sizeof(state) ) return -1;

    memcpy( (void *)&state, in, sizeof(state) );

    cukHwSetLoadSwitch(state);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t state;

    state = cukHwGetLoadSwitch();

    if( maxoutsize < sizeof(state) ) return -1;

    memcpy( *out, (void *)&state, sizeof(state) );

    return sizeof(state);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetOutputSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t state;

    if( insize != sizeof(state) ) return -1;

    memcpy( (void *)&state, in, sizeof(state) );

    cukHwSetOutputSwitch(state);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetOutputSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t state;

    state = cukHwGetOutputSwitch();

    if( maxoutsize < sizeof(state) ) return -1;

    memcpy( *out, (void *)&state, sizeof(state) );

    return sizeof(state);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    cukConfigMeasGains_t gains;

    if( insize != sizeof(gains) ) return -1;

    memcpy( (void* )&gains, in, sizeof(gains) );

    cukHwSetMeasGains(&gains);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    cukConfigMeasGains_t gains;

    cukHwGetMeasGains(&gains);

    if( maxoutsize < sizeof(gains) ) return -1;

    memcpy( *out, (void *)&gains, sizeof(gains) );

    return sizeof(gains);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    (void)out;
    (void)maxoutsize;

    cukHwStatusClear();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;

    uint32_t status;

    status = cukHwStatus();

    if( maxoutsize < sizeof(status) ) return -1;

    memcpy( *out, (void *)&status, sizeof(status) );

    return sizeof(status);
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetFiltCoef(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    float alpha;

    if( insize != sizeof(alpha) ) return -1;

    memcpy( (void *)&alpha, in, sizeof(alpha) );

    cukHwSetFilterCoef(alpha);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetFiltCoef(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    float alpha;

    alpha = cukHwGetFilterCoef();

    if( maxoutsize < sizeof(alpha) ) return -1;

    memcpy( *out, (void *)&alpha, sizeof(alpha) );

    return sizeof(alpha);
}
//-----------------------------------------------------------------------------
//=============================================================================
