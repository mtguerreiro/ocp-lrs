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
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

// ePWM modules used
#define EPWM_TRIG_BASE      EPWM2_BASE   // EPWM2 → SOCA trigger for ADC (+ visible on EPWM2A)
#define EPWM_PWR_BASE       EPWM4_BASE   // EPWM4 → visible PWM output

// Time-base (UP-count). TBCLK = DEVICE_SYSCLK_FREQ / (HSPCLKDIV*CLKDIV) = /1,/1 here.
#define EPWM2_TBPRD         ((uint16_t)500)     // ADC trigger period (adjust as needed)
#define EPWM4_TBPRD         ((uint16_t)2000)    // PWM period (e.g., ~100 kHz @ 200 MHz TBCLK)

// ADC acquisition window (~75 ns @ 200 MHz). Tune to your analog front-end.
#define ADC_ACQPS           (14U)

// Status bits
#define HW_STATUS_ENABLED   (1UL << 0)

// Small helpers
#define CLAMPF(x,lo,hi)     ((x) < (lo) ? (lo) : ((x) > (hi) ? (hi) : (x)))

//=============================================================================

//=============================================================================
/*------------------------------- Types -------------------------------------*/
//=============================================================================
typedef struct{

    uint32_t status;                // HW status flags (bit0 = PWM enabled)

    uint32_t pwmPeriod;             // Cached TBPRD (ticks) for EPWM4

    buckConfigMeasurements_t meas;  // Latest filtered/scaled measurements (optional)
    buckConfigControl_t      control;// Last applied control (u, etc.)

    buckConfigMeasGains_t gains;    // Measurement gains/offsets

    float alpha;                    // Low-pass filter coefficient [0..1]

}buckHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void buckHwInitializeAdc(void);
static void buckHwInitializePwm(void);
static void buckHwInitializeGpio(void);
static void buckHwInitializeMeasGains(void);

// Internal helpers
static inline uint32_t get_tbclk_hz(void);
static inline uint16_t duty_to_cmpa(uint16_t tbprd, float d);
static inline uint32_t freq_from_tbprd(uint16_t tbprd);

// Minimal ADC ISR (optional placeholder)
static __interrupt void buckHwAdcEocISR(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static buckHwControl_t hwControl = {.pwmPeriod = 0, .status = 0, .alpha = 0.2f};

// Software cache for a user-requested deadtime (no hardware dead-band in this build)
static uint16_t s_deadtimeTicks = 0U;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckHwInit(void *param){

    (void)param;

    buckHwInitializeGpio();
    buckHwInitializePwm();
    buckHwInitializeAdc();
    buckHwInitializeMeasGains();

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckHwStatus(void){

    return (int32_t)hwControl.status;
}
//-----------------------------------------------------------------------------
void buckHwStatusClear(void){

    hwControl.status = 0;
}
//-----------------------------------------------------------------------------
void buckHwSetPwmEnable(uint32_t enable){

    if(enable){
        // Start both EPWM counters (ADC trigger + power PWM)
        EPWM_setTimeBaseCounter(EPWM_TRIG_BASE, 0);
        EPWM_setTimeBaseCounter(EPWM_PWR_BASE,  0);

        EPWM_enableADCTrigger(EPWM_TRIG_BASE, EPWM_SOC_A);

        EPWM_setTimeBaseCounterMode(EPWM_TRIG_BASE, EPWM_COUNTER_MODE_UP);
        EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE,  EPWM_COUNTER_MODE_UP);

        hwControl.status |= HW_STATUS_ENABLED;
    }else{
        // Stop counters and disable SOCA
        EPWM_disableADCTrigger(EPWM_TRIG_BASE, EPWM_SOC_A);

        EPWM_setTimeBaseCounterMode(EPWM_TRIG_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
        EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE,  EPWM_COUNTER_MODE_STOP_FREEZE);

        hwControl.status &= ~HW_STATUS_ENABLED;
    }
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetPwmEnable(void){

    return ( (hwControl.status & HW_STATUS_ENABLED) ? 1U : 0U );
}

//-----------------------------------------------------------------------------
void buckHwSetPwmFrequency(uint32_t freq){

    if(freq == 0U) freq = 1U;

    // Compute new TBPRD from desired frequency (UP-count mode approximation)
    uint32_t tbclk = get_tbclk_hz();
    uint32_t prd   = tbclk / freq;
    if(prd < 2U) prd = 2U;
    if(prd > 65535U) prd = 65535U;

    EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_setTimeBasePeriod(EPWM_PWR_BASE, (uint16_t)prd);

    // Keep the same duty ratio: recompute CMPA from current duty estimate
    uint16_t newPrd = (uint16_t)prd;
    uint16_t oldPrd = hwControl.pwmPeriod ? (uint16_t)hwControl.pwmPeriod : EPWM4_TBPRD;
    uint16_t oldCmp = EPWM_getCounterCompareValue(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A);
    float    duty   = (oldPrd > 0U) ? ((float)oldCmp / (float)oldPrd) : 0.0f;
           duty   = CLAMPF(duty, 0.0f, 0.95f);

    EPWM_setCounterCompareValue(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A,
                                duty_to_cmpa(newPrd, duty));

    if( hwControl.status & HW_STATUS_ENABLED ){
        EPWM_setTimeBaseCounter(EPWM_PWR_BASE, 0);
        EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE, EPWM_COUNTER_MODE_UP);
    }

    hwControl.pwmPeriod = newPrd;
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetPwmFrequency(void){

    uint16_t prd = EPWM_getTimeBasePeriod(EPWM_PWR_BASE);
    if(prd == 0U) prd = (uint16_t)hwControl.pwmPeriod;
    if(prd == 0U) return 0U;

    return freq_from_tbprd(prd);
}
//-----------------------------------------------------------------------------
void buckHwSetPwmDuty(float duty){

    duty = CLAMPF(duty, 0.0f, 0.95f);

    uint16_t prd = EPWM_getTimeBasePeriod(EPWM_PWR_BASE);
    if(prd == 0U) prd = (uint16_t)(hwControl.pwmPeriod ? hwControl.pwmPeriod : EPWM4_TBPRD);

    uint16_t cmpa = duty_to_cmpa(prd, duty);
    if(cmpa >= prd) cmpa = prd - 1U;

    // Shadowed update at CTR=ZERO already set in init
    EPWM_setCounterCompareValue(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A, cmpa);

    // Keep last command available for upper layers
    hwControl.control.u = duty;
}
//-----------------------------------------------------------------------------
float buckHwGetPwmDuty(void){

    // Prefer reading back hardware to reflect the real output
    uint16_t prd = EPWM_getTimeBasePeriod(EPWM_PWR_BASE);
    uint16_t cmp = EPWM_getCounterCompareValue(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A);

    if(prd == 0U) prd = (uint16_t)(hwControl.pwmPeriod ? hwControl.pwmPeriod : EPWM4_TBPRD);
    if(prd == 0U) return 0.0f;

    float duty = (float)cmp / (float)prd;
    return CLAMPF(duty, 0.0f, 0.999f);
}
//-----------------------------------------------------------------------------
void buckHwSetPwmDeadTime(float deadtime){

    // This build does not configure hardware dead-band; keep the software cache.
    if(deadtime < 0.0f) deadtime = 0.0f;
    double ticks = (double)get_tbclk_hz() * (deadtime * 1e-6);
    if(ticks < 0.0) ticks = 0.0;
    if(ticks > 65535.0) ticks = 65535.0;
    s_deadtimeTicks = (uint16_t)(ticks + 0.5);
}
//-----------------------------------------------------------------------------
float buckHwGetPwmDeadTime(void){

    return (float)s_deadtimeTicks / (float)get_tbclk_hz() * 1e6f;
}
//-----------------------------------------------------------------------------
int32_t buckHwGetMeasurements(void *meas){

    if(!meas) return -1;

    // Copy last filtered & scaled values (ISR placeholder currently not updating)
    memcpy(meas, (const void *)&hwControl.meas, sizeof(buckConfigMeasurements_t));

    return (int32_t)sizeof(buckConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t buckHwApplyOutputs(void *outputs, int32_t size){

    if(!outputs || size < (int32_t)sizeof(buckConfigControl_t)) return -1;

    buckConfigControl_t *control = (buckConfigControl_t *)outputs;

    // Apply duty command (0..1)
    buckHwSetPwmDuty(control->u);

    // Keep the whole command structure for traceability
    hwControl.control = *control;

    return 0;
}
//-----------------------------------------------------------------------------
void buckHwDisable(void){

    buckHwSetPwmEnable(0U);
}
//-----------------------------------------------------------------------------
void buckHwEnable(void){

    buckHwSetPwmEnable(1U);
}
//-----------------------------------------------------------------------------
void buckHwControllerDisable(void){

    // Optional: add controller-related status bits if needed
}
//-----------------------------------------------------------------------------
void buckHwControllerEnable(void){

    // Optional: add controller-related status bits if needed
}
//-----------------------------------------------------------------------------
void buckHwSetInputRelay(uint32_t state){

    GPIO_setDirectionMode(8, GPIO_DIR_MODE_OUT);
    GPIO_writePin(8, (state ? 1U : 0U));
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetInputRelay(void){

    // Read actual pin level
    return (uint32_t)GPIO_readPin(8);
}
//-----------------------------------------------------------------------------
void buckHwSetOutputRelay(uint32_t state){

    GPIO_setDirectionMode(9, GPIO_DIR_MODE_OUT);
    GPIO_writePin(9, (state ? 1U : 0U));
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetOutputRelay(void){

    // Read actual pin level
    return (uint32_t)GPIO_readPin(9);
}
//-----------------------------------------------------------------------------
void buckHwSetMeasGains(buckConfigMeasGains_t *gains){

    if(!gains) return;
    hwControl.gains = *gains;
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetMeasGains(buckConfigMeasGains_t *gains){

    if(!gains) return 0U;
    *gains = hwControl.gains;

    return (uint32_t)sizeof(buckConfigMeasGains_t);
}
//-----------------------------------------------------------------------------
void buckHwShutDown(void){

    // Safe shutdown: set duty to zero and disable PWM engines
    buckHwSetPwmDuty(0.0f);
    buckHwDisable();
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
static inline uint32_t get_tbclk_hz(void)
{
    // System clock (e.g., 200 MHz) from device.h
    return DEVICE_SYSCLK_FREQ;
}

static inline uint16_t duty_to_cmpa(uint16_t tbprd, float d)
{
    float dlim = CLAMPF(d, 0.0f, 0.95f);
    float raw  = dlim * (float)tbprd;
    if(raw > (float)(tbprd - 1U)) raw = (float)(tbprd - 1U);
    return (uint16_t)(raw + 0.5f);
}

static inline uint32_t freq_from_tbprd(uint16_t tbprd)
{
    uint32_t tbclk = get_tbclk_hz();
    if(tbprd == 0U) return 0U;
    return tbclk / (uint32_t)tbprd;   // UP-count approx
}

//-----------------------------------------------------------------------------
static void buckHwInitializeAdc(void){

    // Minimal ADC setup: ADCA 12-bit, single-ended, triggered by EPWM2 SOCA
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);
    ADC_setMode(ADCA_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCA_BASE);

    // (Optional) Configure additional ADC modules if needed:
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_4_0);
    ADC_setMode(ADCB_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_enableConverter(ADCB_BASE);

    ADC_setPrescaler(ADCC_BASE, ADC_CLK_DIV_4_0);
    ADC_setMode(ADCC_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
    ADC_enableConverter(ADCC_BASE);

    DEVICE_DELAY_US(1000);

    // SOCs on EPWM2 SOCA (example channels — adapt to your board)
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN0, ADC_ACQPS);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN1, ADC_ACQPS);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN2, ADC_ACQPS);

    // Optionally enable an interrupt if you want to process data each cycle
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER2);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);

    Interrupt_register(INT_ADCA1, buckHwAdcEocISR);
    Interrupt_enable  (INT_ADCA1);
}
//-----------------------------------------------------------------------------
static void buckHwInitializePwm(void){

    // Freeze TBCLK while configuring both ePWMs
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_GTBCLKSYNC);

    // ---------- EPWM2: SOCA trigger + visible waveform on EPWM2A ----------
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM2);

    EPWM_setClockPrescaler(EPWM_TRIG_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
    EPWM_setTimeBaseCounter(EPWM_TRIG_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM_TRIG_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_setTimeBasePeriod(EPWM_TRIG_BASE, EPWM2_TBPRD);

    // SOCA on period, every period
    EPWM_setADCTriggerSource(EPWM_TRIG_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_PERIOD);
    EPWM_enableADCTrigger(EPWM_TRIG_BASE, EPWM_SOC_A);

    // Make EPWM2A visible on the pin (50% duty, same style as EPWM4A)
    EPWM_setCounterCompareShadowLoadMode(EPWM_TRIG_BASE, EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareValue(EPWM_TRIG_BASE, EPWM_COUNTER_COMPARE_A, EPWM2_TBPRD/2);

    EPWM_setActionQualifierAction(EPWM_TRIG_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM_TRIG_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

    EPWM_setTimeBaseCounterMode(EPWM_TRIG_BASE, EPWM_COUNTER_MODE_UP);

    // ---------- EPWM4: visible PWM ----------
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_EPWM4);

    EPWM_setClockPrescaler(EPWM_PWR_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_1);
    EPWM_setTimeBaseCounter(EPWM_PWR_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_setTimeBasePeriod(EPWM_PWR_BASE, EPWM4_TBPRD);

    // Shadow CMPA @ ZERO and start at 50% duty (visible right away)
    EPWM_setCounterCompareShadowLoadMode(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareValue(EPWM_PWR_BASE, EPWM_COUNTER_COMPARE_A, EPWM4_TBPRD/2);

    // TI-style AQ: HIGH at ZERO, LOW at UP_CMPA → clean pulse on A
    EPWM_setActionQualifierAction(EPWM_PWR_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM_PWR_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

    // Optional complementary output on B (inverse AQ)
    EPWM_setActionQualifierAction(EPWM_PWR_BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_LOW,  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM_PWR_BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);

    // Unfreeze TBCLK
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_GTBCLKSYNC);
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    // Start counters now so both waveforms are visible immediately
    EPWM_setTimeBaseCounter(EPWM_TRIG_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM_TRIG_BASE, EPWM_COUNTER_MODE_UP);

    EPWM_setTimeBaseCounter(EPWM_PWR_BASE, 0);
    EPWM_setTimeBaseCounterMode(EPWM_PWR_BASE, EPWM_COUNTER_MODE_UP);

    // Cache TBPRD (EPWM4 is the power PWM we modify later)
    hwControl.pwmPeriod = EPWM_getTimeBasePeriod(EPWM_PWR_BASE);

    // Set "enabled" status since we start running by default
    hwControl.status |= HW_STATUS_ENABLED;
}
//-----------------------------------------------------------------------------
static void buckHwInitializeGpio(void){

    // GPIO8/9: relay outputs (CPU1 must assign ownership to CPU2 beforehand)
    GPIO_setDirectionMode(8, GPIO_DIR_MODE_OUT);
    GPIO_writePin(8, 0);
    GPIO_setDirectionMode(9, GPIO_DIR_MODE_OUT);
    GPIO_writePin(9, 0);
}
//-----------------------------------------------------------------------------
static void buckHwInitializeMeasGains(void){

    // Default gains/offsets from your config header
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

// Minimal ADC ISR (placeholder: clears flags only; add filtering if needed)
static __interrupt void buckHwAdcEocISR(void)
{
    // Touch last conversion result if you want to ensure EOC path is serviced
    (void)ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2);

    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
