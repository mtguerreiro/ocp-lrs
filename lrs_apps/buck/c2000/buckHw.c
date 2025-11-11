/*
 * buckHw.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckHw.h"

#include "driverlib.h"
#include "device.h"

#include "c2000Config.h"
#include "buckConfig.h"

#include <stdint.h>
#include <string.h>

/* Open controller project */
#include "ocpConfig.h"
#include "ocp/ocpTrace.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

// ePWM bases
#define EPWM_TRIG_BASE          EPWM2_BASE   
#define EPWM_PWR_BASE           EPWM4_BASE   

#define GPIO_DEBUG_PIN          C2000_CONFIG_CPU2_GPIO_DEBUG

// Relay GPIOs
#define GPIO_RELAY1_PIN         C2000_CONFIG_CPU2_GPIO_INPUT_RELAY
#define GPIO_RELAY2_PIN         C2000_CONFIG_CPU2_GPIO_OUTPUT_RELAY

// Load switch
#define GPIO_LOAD_SWITCH_PIN    C2000_CONFIG_CPU2_GPIO_LOAD_SWITCH

// Time-base (UP-count)
#define EPWM2_TBPRD             ((uint16_t)1000 - 1)     // ADC trigger period
#define EPWM4_TBPRD             ((uint16_t)1000 - 1)    // Power PWM period

// ADC acquisition
#define ADC_ACQPS               (25U)               

// Forward for ISR
static __interrupt void buckHwAdcA_ISR(void);
//=============================================================================

//=============================================================================
/*------------------------------- Structures --------------------------------*/
//=============================================================================
typedef struct{
    uint32_t status;                 
    uint32_t pwmPeriod;              
    buckConfigMeasurements_t meas;   
    buckConfigControl_t      control;
    buckConfigMeasGains_t    gains;
    float alpha;
    void (*adcCallback)(void);
    float loadSwitch;
} buckHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void buckHwInitializeAdc(void *adcCallback);
static void buckHwInitializePwm(void);
static void buckHwInitializeGpio(void);
static void buckHwInitializeMeasGains(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static buckHwControl_t hwControl = {.pwmPeriod = 0, .status = 0, .alpha = 0.2f, .loadSwitch=0.0f};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckHwInit(void *adcCallback)
{

    buckHwInitializeAdc(adcCallback);
    buckHwInitializePwm();
    buckHwInitializeGpio();
    buckHwInitializeMeasGains();

    hwControl.pwmPeriod = EPWM_getTimeBasePeriod(EPWM_PWR_BASE);

    /* Adds load switch to the trace so we can trigger on it */
    ocpTraceAddSignal(BUCK_CONFIG_OCP_TRACE_ID, (void *)&hwControl.loadSwitch, "Load switch");

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckHwStatus(void)
{
    
    return (int32_t)hwControl.status;
}
//-----------------------------------------------------------------------------
void buckHwStatusClear(void)
{
    hwControl.status = 0;
}
//-----------------------------------------------------------------------------
void buckHwSetPwmEnable(uint32_t enable)
{
    if(enable)
    {
        // EPWM_clearTripZoneFlag(EPWM_PWR_BASE, EPWM_TZ_FLAG_OST);
        EPWM_setActionQualifierContSWForceAction(EPWM_PWR_BASE,
                                                 EPWM_AQ_OUTPUT_A,
                                                 EPWM_AQ_SW_DISABLED);
        EPWM_setActionQualifierContSWForceAction(EPWM_PWR_BASE,
                                                 EPWM_AQ_OUTPUT_B,
                                                 EPWM_AQ_SW_DISABLED);
    }
    else
    {
        // EPWM_forceTripZoneEvent(EPWM_PWR_BASE, EPWM_TZ_FORCE_EVENT_OST);
        EPWM_setActionQualifierContSWForceAction(EPWM_PWR_BASE,
                                                 EPWM_AQ_OUTPUT_A,
                                                 EPWM_AQ_SW_OUTPUT_LOW);
        EPWM_setActionQualifierContSWForceAction(EPWM_PWR_BASE,
                                                 EPWM_AQ_OUTPUT_B,
                                                 EPWM_AQ_SW_OUTPUT_HIGH);
    }
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetPwmEnable(void)
{
    uint16_t tbctl = HWREGH(EPWM_PWR_BASE + EPWM_O_TBCTL);
    uint16_t mode = tbctl & 0x3U;

    return ((mode == 0U) || (mode == 2U)) ? 1U : 0U;
}
//-----------------------------------------------------------------------------
void buckHwSetPwmFrequency(uint32_t freq)
{
    // if(freq == 0U) freq = 1U;

    // uint32_t tbclk = DEVICE_SYSCLK_FREQ;
    // uint32_t prd   = tbclk / freq;
    // if(prd < 2U)     prd = 2U;
    // if(prd > 65535U) prd = 65535U;

    
    // EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
    // EPWM_setTimeBasePeriod(EPWM_PWR_BASE, (uint16_t)prd);

    
    // uint16_t newPrd = (uint16_t)prd;
    // uint16_t oldPrd = (hwControl.pwmPeriod ? (uint16_t)hwControl.pwmPeriod : EPWM4_TBPRD);
    // uint16_t oldCmp = EPWM_getCounterCompareValue(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A);
    // float    duty   = (oldPrd ? ((float)oldCmp/(float)oldPrd) : 0.5f);
    // if(duty < 0.0f) duty = 0.0f;
    
    // float maxDuty = (newPrd > 0U) ? ((float)(newPrd - 1U) / (float)newPrd) : 0.0f;
    // if(duty > maxDuty) duty = maxDuty;

    // EPWM_setCounterCompareValue(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A,
    //                             (uint16_t)(duty * (float)newPrd + 0.5f));

    
    // if(buckHwGetPwmEnable())
    // {
    //     EPWM_setTimeBaseCounter(EPWM_PWR_BASE, 0);
    //     EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE, EPWM_COUNTER_MODE_UP);
    // }

    // hwControl.pwmPeriod = newPrd;
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetPwmFrequency(void)
{
    // uint16_t prd = EPWM_getTimeBasePeriod(EPWM_PWR_BASE);
    // if(prd == 0U) prd = (uint16_t)hwControl.pwmPeriod;

    // return (prd ? (DEVICE_SYSCLK_FREQ/(uint32_t)prd) : 0U);

    return 0;
}
//-----------------------------------------------------------------------------
void buckHwSetPwmDuty(float duty)
{
    if(duty < 0.0f) duty = 0.0f;

    uint16_t prd = EPWM_getTimeBasePeriod(EPWM_PWR_BASE);
    if(prd == 0U) prd = (uint16_t)(hwControl.pwmPeriod ? hwControl.pwmPeriod : EPWM4_TBPRD);

    
    float maxDuty = (prd > 0U) ? ((float)(prd - 1U) / (float)prd) : 0.0f;
    if(duty > maxDuty) duty = maxDuty;

    uint16_t cmpa = (uint16_t)(duty * (float)prd + 0.5f);
    if(cmpa >= prd) cmpa = prd - 1U;

    EPWM_setCounterCompareValue(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A, cmpa);
}
//-----------------------------------------------------------------------------
float buckHwGetPwmDuty(void)
{
    // uint16_t prd = EPWM_getTimeBasePeriod(EPWM_PWR_BASE);
    // if(prd == 0U) prd = (uint16_t)(hwControl.pwmPeriod ? hwControl.pwmPeriod : EPWM4_TBPRD);

    // uint16_t cmp = EPWM_getCounterCompareValue(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A);
    // return (prd ? ((float)cmp / (float)prd) : 0.0f);

    return 0;
}
//-----------------------------------------------------------------------------
void buckHwSetPwmDeadTime(float deadtime_us)
{
    // if(deadtime_us < 0.0f) deadtime_us = 0.0f;

    
    // double   ticks_d = (double)DEVICE_SYSCLK_FREQ * (deadtime_us * 1e-6);
    // if(ticks_d > 65535.0) ticks_d = 65535.0;
    // uint16_t ticks   = (uint16_t)(ticks_d + 0.5);

    // if(ticks == 0U)
    // {
       
    //     EPWM_setDeadBandDelayMode(EPWM_PWR_BASE, EPWM_DB_RED, false);
    //     EPWM_setDeadBandDelayMode(EPWM_PWR_BASE, EPWM_DB_FED, false);
    //     EPWM_setRisingEdgeDelayCount (EPWM_PWR_BASE, 0U);
    //     EPWM_setFallingEdgeDelayCount(EPWM_PWR_BASE, 0U);
    // }
    // else
    // {
        
    //     EPWM_setRisingEdgeDelayCount (EPWM_PWR_BASE, ticks);
    //     EPWM_setFallingEdgeDelayCount(EPWM_PWR_BASE, ticks);
    //     EPWM_setDeadBandDelayMode(EPWM_PWR_BASE, EPWM_DB_RED, true);
    //     EPWM_setDeadBandDelayMode(EPWM_PWR_BASE, EPWM_DB_FED, true);
        
    // }

    // s_deadtimeTicks = ticks; 
}
//-----------------------------------------------------------------------------
float buckHwGetPwmDeadTime(void)
{
    // return (float)s_deadtimeTicks / (float)DEVICE_SYSCLK_FREQ * 1e6f;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckHwGetMeasurements(void *meas)
{
    /*
     * TODO: add protection as trip, not here
     */

    buckConfigMeasurements_t *dst;
    dst = (buckConfigMeasurements_t *)meas;

    uint16_t il_raw = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0) & 0x0FFF; // IL 
    uint16_t v_in_buck_raw = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1) & 0x0FFF; // V_in_buck
    uint16_t v_in_raw = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2) & 0x0FFF; // V_in

    uint16_t v_out_raw = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0) & 0x0FFF; // V_out 
    uint16_t il_avg_raw = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER1) & 0x0FFF; // IL_avg

    uint16_t v_out_buck_raw = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER0) & 0x0FFF; // V_out_buck

    dst->il = hwControl.gains.il_gain * il_raw + hwControl.gains.il_ofs;
    dst->io = 0;
    dst->v_dc_in = hwControl.gains.v_dc_in_gain * v_in_raw + hwControl.gains.v_dc_in_ofs;
    dst->v_in = hwControl.gains.v_in_gain * v_in_buck_raw + hwControl.gains.v_in_ofs;
    dst->v_dc_out = hwControl.gains.v_dc_out_gain * v_out_buck_raw + hwControl.gains.v_dc_out_ofs;
    dst->v_out = hwControl.gains.v_out_gain * v_out_raw + hwControl.gains.v_out_ofs;

    if( (dst->v_dc_in > BUCK_CONFIG_V_LIM) || (dst->v_in > BUCK_CONFIG_V_LIM) )
        hwControl.status = 1;

    if( (dst->v_dc_out > BUCK_CONFIG_V_LIM) || (dst->v_out > BUCK_CONFIG_V_LIM) )
        hwControl.status = 1;
    
    if( (dst->il > BUCK_CONFIG_I_LIM) || (dst->il < -BUCK_CONFIG_I_LIM) )
        hwControl.status = 1;

    if( hwControl.status != 0 ){
        buckHwShutDown();
        return -1;
    }

    return sizeof(buckConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t buckHwApplyOutputs(void *outputs, int32_t size)
{
    if(!outputs || size < (int32_t)sizeof(buckConfigControl_t)) return -1;

    buckConfigControl_t *control = (buckConfigControl_t *)outputs;
    buckHwSetPwmDuty(control->u);

    return 0;
}
//-----------------------------------------------------------------------------
void buckHwDisable(void){

    EPWM_disableADCTrigger(EPWM_TRIG_BASE, EPWM_SOC_A);

    EPWM_setTimeBaseCounterMode(EPWM_TRIG_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);

    buckHwSetPwmEnable(0);
}
//-----------------------------------------------------------------------------
void buckHwEnable(void){

    EPWM_enableADCTrigger(EPWM_TRIG_BASE, EPWM_SOC_A);

    EPWM_setTimeBaseCounter(EPWM_TRIG_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM_TRIG_BASE, EPWM_COUNTER_MODE_UP);

    EPWM_setTimeBaseCounter(EPWM_PWR_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE, EPWM_COUNTER_MODE_UP);
}
//-----------------------------------------------------------------------------
void buckHwControllerDisable(void){

    buckHwSetPwmEnable(0);
}
//-----------------------------------------------------------------------------
void buckHwControllerEnable(void){

    buckHwSetPwmEnable(1);
}
//-----------------------------------------------------------------------------
void buckHwSetInputRelay(uint32_t state)
{
    GPIO_writePin(GPIO_RELAY1_PIN, (state ? 1U : 0U));
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetInputRelay(void)
{
    return (uint32_t)GPIO_readPin(GPIO_RELAY1_PIN);
}
//-----------------------------------------------------------------------------
void buckHwSetOutputRelay(uint32_t state)
{   
    GPIO_writePin(GPIO_RELAY2_PIN, (state ? 1U : 0U));
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetOutputRelay(void)
{
    return (uint32_t)GPIO_readPin(GPIO_RELAY2_PIN);
}
//-----------------------------------------------------------------------------
void buckHwSetLoadSwitch(uint32_t state)
{   
    hwControl.loadSwitch = (float)(state & 0x01);
    GPIO_writePin(GPIO_LOAD_SWITCH_PIN, (state ? 0U : 1U));
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetLoadSwitch(void)
{
    return (uint32_t)GPIO_readPin(GPIO_LOAD_SWITCH_PIN);
}
//-----------------------------------------------------------------------------
void buckHwSetMeasGains(buckConfigMeasGains_t *gains)
{
    if(!gains) return;
    hwControl.gains = *gains;
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetMeasGains(buckConfigMeasGains_t *gains)
{
    if(!gains) return 0U;
    *gains = hwControl.gains;
    return sizeof(buckConfigMeasGains_t);
}
//-----------------------------------------------------------------------------
void buckHwShutDown(void)
{
    buckHwSetInputRelay(0);
    buckHwSetOutputRelay(0);
    buckHwSetLoadSwitch(0);
    buckHwSetPwmDuty(0.0f);
    buckHwSetPwmEnable(0);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void buckHwInitializeAdc(void *adcCallback)
{

    hwControl.adcCallback = (void (*)(void))adcCallback;

    //  ADCA / ADCB / ADCC 
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
    ADC_setMode     (ADCA_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(1000);

    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_4_0);
    ADC_setMode     (ADCB_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCB_BASE);
    DEVICE_DELAY_US(1000);

    ADC_setPrescaler(ADCC_BASE, ADC_CLK_DIV_4_0);
    ADC_setMode     (ADCC_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setInterruptPulseMode(ADCC_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCC_BASE);
    DEVICE_DELAY_US(1000);

    // SOC mapping (triggered by EPWM2 SOCA)
    //  ADCIN_A5  -> ADCA_SOC0  (Buffer 0) : IL
    //  ADCIN_A4  -> ADCA_SOC1  (Buffer 1) : V_in_buck
    //  ADCIN_A1  -> ADCA_SOC2  (Buffer 2) : V_in
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN5, ADC_ACQPS); 
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN4, ADC_ACQPS);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN1, ADC_ACQPS);
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER2);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);

    //  ADCIN_B4  -> ADCB_SOC0  : V_out
    //  ADCIN_B5  -> ADCB_SOC1  : IL_avg
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN4, ADC_ACQPS);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN5, ADC_ACQPS);

    //  ADCIN_C4  -> ADCC_SOC0  : V_out_buck
    ADC_setupSOC(ADCC_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN4, ADC_ACQPS);

    // ---------- ISR registration ----------
    Interrupt_register(INT_ADCA1, buckHwAdcA_ISR);
    Interrupt_enable  (INT_ADCA1);
    
    Interrupt_enable  (INTERRUPT_CPU_INT1);
}
//-----------------------------------------------------------------------------
static void buckHwInitializePwm(void)
{

    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    /*
     * TODO:
     * - Add trip zone
     * - Define state of outputs when disabled/tripped
     */
    // ---------- EPWM2: SOCA trigger + visible EPWM2A ----------
    EPWM_setClockPrescaler(EPWM_TRIG_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
    EPWM_setTimeBaseCounter(EPWM_TRIG_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM_TRIG_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_setTimeBasePeriod(EPWM_TRIG_BASE, EPWM2_TBPRD);

    EPWM_setADCTriggerSource(EPWM_TRIG_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_PERIOD);
    EPWM_setADCTriggerEventPrescale(EPWM_TRIG_BASE, EPWM_SOC_A, 1U);
    EPWM_enableADCTrigger(EPWM_TRIG_BASE, EPWM_SOC_A);

    EPWM_setCounterCompareShadowLoadMode(EPWM_TRIG_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareValue(EPWM_TRIG_BASE, EPWM_COUNTER_COMPARE_A, EPWM2_TBPRD/2);
    EPWM_setActionQualifierAction(EPWM_TRIG_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM_TRIG_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

    //EPWM_setTimeBaseCounterMode(EPWM_TRIG_BASE, EPWM_COUNTER_MODE_UP);

    // ---------- EPWM4: visible PWM ----------
    EPWM_setClockPrescaler(EPWM_PWR_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
    EPWM_setTimeBaseCounter(EPWM_PWR_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_setTimeBasePeriod(EPWM_PWR_BASE, EPWM4_TBPRD);

    EPWM_setSyncPulseSource(EPWM_PWR_BASE, HRPWM_PWMSYNC_SOURCE_ZERO);

    // EPWM_enableTripZoneSignals(EPWM_PWR_BASE, EPWM_TZ_SIGNAL_OSHT1);
    // EPWM_setTripZoneAction(EPWM_PWR_BASE, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_HIGH);
    // EPWM_setTripZoneAction(EPWM_PWR_BASE, EPWM_TZ_ACTION_EVENT_TZB, EPWM_TZ_ACTION_HIGH);
    // EPWM_forceTripZoneEvent(EPWM_PWR_BASE, EPWM_TZ_FORCE_EVENT_OST);

    EPWM_setCounterCompareShadowLoadMode(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareValue(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A, 0);

    EPWM_setActionQualifierAction(EPWM_PWR_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM_PWR_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

    EPWM_setActionQualifierAction(EPWM_PWR_BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM_PWR_BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_HIGH,  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

    EPWM_setDeadBandDelayMode(EPWM_PWR_BASE, EPWM_DB_RED, true);
    EPWM_setDeadBandDelayMode(EPWM_PWR_BASE, EPWM_DB_FED, true);

    EPWM_setDeadBandDelayPolarity(EPWM_PWR_BASE, EPWM_DB_RED, EPWM_DB_POLARITY_ACTIVE_LOW);

    EPWM_setRisingEdgeDeadBandDelayInput(EPWM_PWR_BASE, EPWM_DB_INPUT_EPWMA); 
    EPWM_setFallingEdgeDeadBandDelayInput(EPWM_PWR_BASE, EPWM_DB_INPUT_EPWMA);
    
    EPWM_setRisingEdgeDelayCount(EPWM_PWR_BASE, 20);
    EPWM_setFallingEdgeDelayCount(EPWM_PWR_BASE, 50);

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE, EPWM_COUNTER_MODE_UP);
}
//-----------------------------------------------------------------------------
static void buckHwInitializeGpio(void)
{
    GPIO_writePin(GPIO_DEBUG_PIN, 0);
    GPIO_writePin(GPIO_RELAY1_PIN, 0);
    GPIO_writePin(GPIO_RELAY2_PIN, 0);

    GPIO_writePin(GPIO_LOAD_SWITCH_PIN, 1); // Load switch has inverted logic
    hwControl.loadSwitch = 0.0f;
}
//-----------------------------------------------------------------------------
static void buckHwInitializeMeasGains(void)
{
    hwControl.gains.io_gain       = BUCK_CONFIG_IO_AVG_GAIN;
    hwControl.gains.io_ofs        = BUCK_CONFIG_IO_AVG_OFFS;
    hwControl.gains.il_gain       = BUCK_CONFIG_IL_GAIN;
    hwControl.gains.il_ofs        = BUCK_CONFIG_IL_OFFS;
    hwControl.gains.v_dc_out_gain = BUCK_CONFIG_V_DC_OUT_GAIN;
    hwControl.gains.v_dc_out_ofs  = BUCK_CONFIG_V_DC_OUT_OFFS;
    hwControl.gains.v_out_gain    = BUCK_CONFIG_V_OUT_GAIN;
    hwControl.gains.v_out_ofs     = BUCK_CONFIG_V_OUT_OFFS;
    hwControl.gains.v_dc_in_gain  = BUCK_CONFIG_V_DC_IN_GAIN;
    hwControl.gains.v_dc_in_ofs   = BUCK_CONFIG_V_DC_IN_OFFS;
    hwControl.gains.v_in_gain     = BUCK_CONFIG_V_IN_GAIN;
    hwControl.gains.v_in_ofs      = BUCK_CONFIG_V_IN_OFFS;
}
//-----------------------------------------------------------------------------
__interrupt void buckHwAdcA_ISR(void)
{

    GPIO_writePin(GPIO_DEBUG_PIN, 1);
    hwControl.adcCallback();

    ADC_clearInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);

    GPIO_writePin(GPIO_DEBUG_PIN, 0);
}
//-----------------------------------------------------------------------------
//=============================================================================

