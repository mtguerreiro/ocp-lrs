/*
 * fsbuckboostHwIf.c
 *
 */
//#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostHwIf.h"

#include "fsbuckboostHw.h"

#include "fsbuckboostConfig.h"

#include "rp/rp.h"
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

    uint32_t reset;

    reset = *( (uint32_t *)in ) & 0x01;

    fsbuckboostHwSetPwmReset(reset);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t reset;

    reset = fsbuckboostHwGetPwmReset();

    *o = reset;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    fsbuckboostHwSetPwmOutputEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

    enable = fsbuckboostHwGetPwmOutputEnable();

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    fsbuckboostHwSetPwmOvfTriggerEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

    enable = fsbuckboostHwGetPwmOvfTriggerEnable();

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    fsbuckboostHwSetPwmInv(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

    enable = fsbuckboostHwGetPwmInv();

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t mode;

    mode = *( (uint32_t *)in ) & 0x01;

    fsbuckboostHwSetPwmMode(mode);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t mode;

    mode = fsbuckboostHwGetPwmMode();

    *o = mode;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmLsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in ) & 0x01;

    fsbuckboostHwSetPwmLsSw(state);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmLsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = fsbuckboostHwGetPwmLsSw();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmHsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in ) & 0x01;

    fsbuckboostHwSetPwmHsSw(state);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmHsSw(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = fsbuckboostHwGetPwmHsSw();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

    fsbuckboostHwSetPwmFrequency(freq);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t freq;

    freq = fsbuckboostHwGetPwmFrequency();

    *o = freq;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float duty;

    duty = *( (float *)in );

    fsbuckboostHwSetPwmDuty(duty);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *o = (float *)*out;
    float duty;

    duty = fsbuckboostHwGetPwmDuty();

    *o = duty;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float deadtime;

    deadtime = *( (float *)in );

    fsbuckboostHwSetPwmDeadTime(deadtime);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *o = (float *)*out;
    float deadtime;

    deadtime = fsbuckboostHwGetPwmDeadTime();

    *o = deadtime;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    fsbuckboostHwSetAdcEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

    enable = fsbuckboostHwGetAdcEnable();

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t trigger;

    trigger = *( (uint32_t *)in ) & 0x01;

    fsbuckboostHwSetAdcManualTrigger(trigger);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t trigger;

    trigger = fsbuckboostHwGetAdcManualTrigger();

    *o = trigger;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    fsbuckboostHwSetAdcInterruptEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

    enable = fsbuckboostHwGetAdcInterruptEnable();

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

    fsbuckboostHwSetAdcSpiFreq(freq);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t freq;

    freq = fsbuckboostHwGetAdcSpiFreq();

    *o = freq;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    fsbuckboostHwSetInputRelay(state);
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = fsbuckboostHwGetInputRelay();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    fsbuckboostHwSetOutputRelay(state);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = fsbuckboostHwGetOutputRelay();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    fsbuckboostHwSetLoadSwitch(state);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = fsbuckboostHwGetLoadSwitch();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    fsbuckboostConfigMeasGains_t *gains;

    gains = ( (fsbuckboostConfigMeasGains_t *)in );

    fsbuckboostHwSetMeasGains(gains);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    fsbuckboostConfigMeasGains_t *o = (fsbuckboostConfigMeasGains_t *)*out;
    fsbuckboostConfigMeasGains_t gains;
    uint32_t size;

    size = fsbuckboostHwGetMeasGains(&gains);

    *o = gains;

    return size;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    fsbuckboostHwStatusClear();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t fsbuckboostHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t status;
    uint32_t *o = (uint32_t *)*out;

    status = fsbuckboostHwStatus();

    *o = status;

    return 4;
}
//-----------------------------------------------------------------------------
//=============================================================================
//#endif /* SOC_CPU1 */
