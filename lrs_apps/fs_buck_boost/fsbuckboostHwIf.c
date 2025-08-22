/*
 * fsbuckboostHwIf.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostHwIf.h"

#include "fsbuckboostHw.h"

#include "fsbuckboostConfig.h"

#include "rp/rp.h"

#include "string.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    rphandle_t handles[FS_BUCK_BOOST_HW_IF_END];
    rpctx_t rp;
}fsbuckboostHwIf_t;

typedef struct{

    fsbuckboostHwIf_t interface;
}fsbuckboostHwIfControl_t;

static fsbuckboostHwIfControl_t hwControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t fsbuckboostHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetPwmMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetPwmMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetPwmLsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetPwmLsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetPwmHsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetPwmHsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t fsbuckboostHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t fsbuckboostHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t fsbuckboostHwIfInitialize(void){

    /* Initializes the request processor */
    rpInitialize(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_END, hwControl.interface.handles);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_PWM_RESET, fsbuckboostHwIfSetPwmReset);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_PWM_RESET, fsbuckboostHwIfGetPwmReset);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_PWM_OUTPUT_ENABLE, fsbuckboostHwIfSetPwmOutputEnable);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_PWM_OUTPUT_ENABLE, fsbuckboostHwIfGetPwmOutputEnable);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_PWM_OVF_TRIGGER_ENABLE, fsbuckboostHwIfSetPwmOvfTriggerEnable);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_PWM_OVF_TRIGGER_ENABLE, fsbuckboostHwIfGetPwmOvfTriggerEnable);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_PWM_INV, fsbuckboostHwIfSetPwmInv);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_PWM_INV, fsbuckboostHwIfGetPwmInv);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_PWM_MODE, fsbuckboostHwIfSetPwmMode);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_PWM_MODE, fsbuckboostHwIfGetPwmMode);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_PWM_LS_SW, fsbuckboostHwIfSetPwmLsSw);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_PWM_LS_SW, fsbuckboostHwIfGetPwmLsSw);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_PWM_HS_SW, fsbuckboostHwIfSetPwmHsSw);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_PWM_HS_SW, fsbuckboostHwIfGetPwmHsSw);
    
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_PWM_FREQ, fsbuckboostHwIfSetPwmFrequency);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_PWM_FREQ, fsbuckboostHwIfGetPwmFrequency);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_PWM_DUTY, fsbuckboostHwIfSetPwmDutyCycle);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_PWM_DUTY, fsbuckboostHwIfGetPwmDutyCycle);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_PWM_DEAD_TIME, fsbuckboostHwIfSetPwmDeadTime);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_PWM_DEAD_TIME, fsbuckboostHwIfGetPwmDeadTime);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_ADC_ENABLE, fsbuckboostHwIfSetAdcEnable);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_ADC_ENABLE, fsbuckboostHwIfGetAdcEnable);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_ADC_MANUAL_TRIG, fsbuckboostHwIfSetAdcManualTrig);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_ADC_MANUAL_TRIG, fsbuckboostHwIfGetAdcManualTrig);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_ADC_INT_ENABLE, fsbuckboostHwIfSetAdcInterruptEnable);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_ADC_INT_ENABLE, fsbuckboostHwIfGetAdcInterruptEnable);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_ADC_SPI_FREQ, fsbuckboostHwIfSetAdcSpiFreq);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_ADC_SPI_FREQ, fsbuckboostHwIfGetAdcSpiFreq);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_INPUT_RELAY, fsbuckboostHwIfSetInputRelay);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_INPUT_RELAY, fsbuckboostHwIfGetInputRelay);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_OUTPUT_RELAY, fsbuckboostHwIfSetOutputRelay);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_OUTPUT_RELAY, fsbuckboostHwIfGetOutputRelay);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_LOAD_SW, fsbuckboostHwIfSetLoadSwitch);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_LOAD_SW, fsbuckboostHwIfGetLoadSwitch);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_SET_MEAS_GAINS, fsbuckboostHwIfSetMeasGains);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_MEAS_GAINS, fsbuckboostHwIfGetMeasGains);

    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_CLEAR_STATUS, fsbuckboostHwIfClearStatus);
    rpRegisterHandle(&hwControl.interface.rp, FS_BUCK_BOOST_HW_IF_GET_STATUS, fsbuckboostHwIfGetStatus);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

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
static int32_t fsbuckboostHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t reset;

    if( insize != sizeof(reset) ) return -1;

    memcpy( (void *)&reset, in, sizeof(reset) );

    fsbuckboostHwSetPwmReset(reset);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t reset;

    reset = fsbuckboostHwGetPwmReset();

    if( maxoutsize < sizeof(reset) ) return -1;

    memcpy( *out, (void *)&reset, sizeof(reset) );

    return sizeof(reset);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t enable;

    if( insize != sizeof(enable) ) return -1;

    memcpy( (void *)&enable, in, sizeof(enable) );

    fsbuckboostHwSetPwmOutputEnable(enable & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t enable;

    enable = fsbuckboostHwGetPwmOutputEnable();

    if( maxoutsize < sizeof(enable) ) return -1;

    memcpy( *out, (void *)&enable, sizeof(enable) );

    return sizeof(enable);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t enable;

    if( insize != sizeof(enable) ) return -1;

    memcpy( (void *)&enable, in, sizeof(enable) );

    fsbuckboostHwSetPwmOvfTriggerEnable(enable & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t enable;

    enable = fsbuckboostHwGetPwmOvfTriggerEnable();

    if( maxoutsize < sizeof(enable) ) return -1;

    memcpy( *out, (void *)&enable, sizeof(enable) );

    return sizeof(enable);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t inv;

    if( insize != sizeof(inv) ) return -1;

    memcpy( (void *)&inv, in, sizeof(inv) );

    fsbuckboostHwSetPwmInv(inv & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t inv;

    inv = fsbuckboostHwGetPwmInv();

    if( maxoutsize < sizeof(inv) ) return -1;

    memcpy( *out, (void *)&inv, sizeof(inv) );

    return sizeof(inv);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t mode;

    if( insize != sizeof(mode) ) return -1;

    memcpy( (void *)&mode, in, sizeof(mode) );

    fsbuckboostHwSetPwmMode(mode & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t mode;

    mode = fsbuckboostHwGetPwmMode();

    if( maxoutsize < sizeof(mode) ) return -1;

    memcpy( *out, (void *)&mode, sizeof(mode) );

    return sizeof(mode);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmLsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t state;

    if( insize != sizeof(state) ) return -1;

    memcpy( (void *)&state, in, sizeof(state) );

    fsbuckboostHwSetPwmLsSw(state & 0x01 );

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmLsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t state;

    state = fsbuckboostHwGetPwmLsSw();

    if( maxoutsize < sizeof(state) ) return -1;

    memcpy( *out, (void *)&state, sizeof(state) );

    return sizeof(state);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmHsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t state;

    if( insize != sizeof(state) ) return -1;

    memcpy( (void *)&state, in, sizeof(state) );

    fsbuckboostHwSetPwmHsSw(state & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmHsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t state;

    state = fsbuckboostHwGetPwmHsSw();

    if( maxoutsize < sizeof(state) ) return -1;

    memcpy( *out, (void *)&state, sizeof(state) );

    return sizeof(state);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t freq;

    if( insize != sizeof(freq) ) return -1;

    memcpy( (void *)&freq, in, sizeof(freq) );

    fsbuckboostHwSetPwmFrequency(freq);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t freq;

    freq = fsbuckboostHwGetPwmFrequency();

    if( maxoutsize < sizeof(freq) ) return -1;

    memcpy( *out, (void *)&freq, sizeof(freq) );

    return sizeof(freq);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    float duty;

    if( insize != sizeof(duty) ) return -1;

    memcpy( (void *)&duty, in, sizeof(duty) );

    fsbuckboostHwSetPwmDuty(duty);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    float duty;

    duty = fsbuckboostHwGetPwmDuty();

    if( maxoutsize < sizeof(duty) ) return -1;

    memcpy( *out, (void *)&duty, sizeof(duty) );

    return sizeof(duty);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    float deadtime;

    if( insize != sizeof(deadtime) ) return -1;

    memcpy( (void *)&deadtime, in, sizeof(deadtime) );

    fsbuckboostHwSetPwmDeadTime(deadtime);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    float deadtime;

    deadtime = fsbuckboostHwGetPwmDeadTime();

    if( maxoutsize < sizeof(deadtime) ) return -1;

    memcpy( *out, (void *)&deadtime, sizeof(deadtime) );

    return sizeof(deadtime);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t enable;

    if( insize != sizeof(enable) ) return -1;

    memcpy( (void *)&enable, in, sizeof(enable) );

    fsbuckboostHwSetAdcEnable(enable & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t enable;

    enable = fsbuckboostHwGetAdcEnable();

    if( maxoutsize < sizeof(enable) ) return -1;

    memcpy( *out, (void *)&enable, sizeof(enable) );

    return sizeof(enable);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t trigger;

    if( insize != sizeof(trigger) ) return -1;

    memcpy( (void *)&trigger, in, sizeof(trigger) );

    fsbuckboostHwSetAdcManualTrigger(trigger & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t trigger;

    trigger = fsbuckboostHwGetAdcManualTrigger();

    if( maxoutsize < sizeof(trigger) ) return -1;

    memcpy( *out, (void *)&trigger, sizeof(trigger) );

    return sizeof(trigger);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t enable;

    if( insize != sizeof(enable) ) return -1;

    memcpy( (void *)&enable, in, sizeof(enable) );

    fsbuckboostHwSetAdcInterruptEnable(enable  & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t enable;

    enable = fsbuckboostHwGetAdcInterruptEnable();

    if( maxoutsize < sizeof(enable) ) return -1;

    memcpy( *out, (void *)&enable, sizeof(enable) );

    return sizeof(enable);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t freq;

    if( insize != sizeof(freq) ) return -1;

    memcpy( (void *)&freq, in, sizeof(freq) );

    fsbuckboostHwSetAdcSpiFreq(freq);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t freq;

    freq = fsbuckboostHwGetAdcSpiFreq();

    if( maxoutsize < sizeof(freq) ) return -1;

    memcpy( *out, (void *)&freq, sizeof(freq) );

    return sizeof(freq);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t state;

    if( insize != sizeof(state) ) return -1;

    memcpy( (void *)&state, in, sizeof(state) );

    fsbuckboostHwSetInputRelay(state);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t state;

    state = fsbuckboostHwGetInputRelay();

    if( maxoutsize < sizeof(state) ) return -1;

    memcpy( *out, (void *)&state, sizeof(state) );

    return sizeof(state);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t state;

    if( insize != sizeof(state) ) return -1;

    memcpy( (void *)&state, in, sizeof(state) );

    fsbuckboostHwSetOutputRelay(state);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t state;

    state = fsbuckboostHwGetOutputRelay();

    if( maxoutsize < sizeof(state) ) return -1;

    memcpy( *out, (void *)&state, sizeof(state) );

    return sizeof(state);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t state;

    if( insize != sizeof(state) ) return -1;

    memcpy( (void *)&state, in, sizeof(state) );

    fsbuckboostHwSetLoadSwitch(state);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t state;

    state = fsbuckboostHwGetLoadSwitch();

    if( maxoutsize < sizeof(state) ) return -1;

    memcpy( *out, (void *)&state, sizeof(state) );

    return sizeof(state);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    fsbuckboostConfigMeasGains_t gains;

    if( insize != sizeof(gains) ) return -1;

    memcpy( (void* )&gains, in, sizeof(gains) );

    fsbuckboostHwSetMeasGains(&gains);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    fsbuckboostConfigMeasGains_t gains;

    fsbuckboostHwGetMeasGains(&gains);

    if( maxoutsize < sizeof(gains) ) return -1;

    memcpy( *out, (void *)&gains, sizeof(gains) );

    return sizeof(gains);
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    (void)out;
    (void)maxoutsize;

    fsbuckboostHwStatusClear();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    (void)out;
    (void)maxoutsize;

    uint32_t status;

    status = fsbuckboostHwStatus();

    if( maxoutsize < sizeof(status) ) return -1;

    memcpy( *out, (void *)&status, sizeof(status) );

    return sizeof(status);
}
//-----------------------------------------------------------------------------
//=============================================================================
