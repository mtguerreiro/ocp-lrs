/*
 * cukHw.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukHw.h"

#include "xparameters.h"
#include "zynqAxiPwm.h"
#include "zynqAxiAdc.h"

#include "cukConfig.h"

#include "zynqConfig.h"

#include "xgpio.h"

#include "utils/dfilt.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define CUK_HW_CONFIG_ADC_SPI_FREQ_HZ       ((uint32_t)16666666)
#define CUK_HW_CONFIG_PWM_FREQ_HZ           ((uint32_t) 100000 )
#define CUK_HW_CONFIG_PWM_DEAD_TIME_NS      ((float) 75e-9 )
#define CUK_HW_CONFIG_PWM_BASE              XPAR_AXI_PWM_0_BASEADDR
#define CUK_HW_CONFIG_ADC_BASE              XPAR_ADC_PSCTL_0_BASEADDR

#define CUK_HW_CONFIG_IRQ_PL_CPU1           ZYNQ_CONFIG_IRQ_PL_TO_CPU1
#define CUK_HW_CONFIG_IRQ_PL_CPU1_PRIO      ZYNQ_CONFIG_IRQ_PL_TO_CPU1_PRIO
#define CUK_HW_CONFIG_ADC_BUFFER            ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR

#define CUK_HW_CONFIG_GPIO_BASE             XPAR_XGPIO_0_BASEADDR
#define CUK_HW_CONFIG_GPIO_CHANNEL          1

#define CUK_HW_CONFIG_GPIO_OUTPUT_OFFS      (1U)
#define CUK_HW_CONFIG_GPIO_OUTPUT           (1 << CUK_HW_CONFIG_GPIO_OUTPUT_OFFS)
#define CUK_HW_CONFIG_GPIO_LOAD_OFFS        (0U)
#define CUK_HW_CONFIG_GPIO_LOAD             (1 << CUK_HW_CONFIG_GPIO_LOAD_OFFS)

/* PWM peripheral clock, in Hz */
#define CUK_HW_PWM_CLK                      100000000
#define CUK_HW_ADC_CLK                      100000000

typedef struct{

    uint32_t status;

    uint32_t pwmPeriod;

    cukConfigMeasurements_t meas;
    cukConfigControl_t control;

    XGpio gpio;

    cukConfigMeasGains_t gains;

    float alpha;

}cukHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void cukHwInitializeAdc(void *intc, cukHwAdcIrqHandle_t irqhandle);
static void cukHwInitializePwm(void);
static void cukHwInitializeGpio(void);
static void cukHwInitializeMeasGains(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static cukHwControl_t hwControl = {.pwmPeriod = 0, .status = 0, .alpha = 0.2f};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukHwInitialize(cukHwInitConfig_t *config){

    cukHwInitializeAdc(config->intc, config->irqhandle);
    cukHwInitializePwm();
    cukHwInitializeGpio();
    cukHwInitializeMeasGains();

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukHwStatus(void){

    return hwControl.status;
}
//-----------------------------------------------------------------------------
void cukHwStatusClear(void){

    hwControl.status = 0;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmReset(uint32_t reset){

    zynqAxiPwmResetWrite(CUK_HW_CONFIG_PWM_BASE, reset);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmReset(void){

    return zynqAxiPwmResetRead(CUK_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetPwmOutputEnable(uint32_t enable){

    zynqAxiPwmOutputEnableWrite(CUK_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmOutputEnable(void){

    return zynqAxiPwmOutputEnableRead(CUK_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetPwmOvfTriggerEnable(uint32_t enable){

    zynqAxiPwmOvfTriggerEnableWrite(CUK_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmOvfTriggerEnable(void){

    return zynqAxiPwmOvfTriggerEnableRead(CUK_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetPwmFrequency(uint32_t freq){

    uint32_t period;

    period = CUK_HW_PWM_CLK / (freq << 1);

    hwControl.pwmPeriod = period;

    zynqAxiPwmPeriodWrite(CUK_HW_CONFIG_PWM_BASE, period);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmFrequency(void){

    uint32_t freq, period;

    period = zynqAxiPwmPeriodRead(CUK_HW_CONFIG_PWM_BASE);

    freq = CUK_HW_PWM_CLK / (period << 1);

    return freq;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmDuty(float duty){

    uint32_t dutyInt;

    dutyInt = (uint32_t)( duty * ((float)hwControl.pwmPeriod ) );

    zynqAxiPwmDutyWrite(CUK_HW_CONFIG_PWM_BASE, dutyInt);
}
//-----------------------------------------------------------------------------
float cukHwGetPwmDuty(void){

    uint32_t dutyInt;
    float duty;

    dutyInt = zynqAxiPwmDutyRead(CUK_HW_CONFIG_PWM_BASE);
    duty = ( (float)dutyInt ) / ( (float)hwControl.pwmPeriod );

    return duty;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmDeadTime(float deadtime){

    uint32_t deadtimeInt;

    deadtimeInt = (uint32_t)( deadtime * ((float)CUK_HW_PWM_CLK) );

    zynqAxiPwmDeadTimeWrite(CUK_HW_CONFIG_PWM_BASE, deadtimeInt);
}
//-----------------------------------------------------------------------------
float cukHwGetPwmDeadTime(void){

    uint32_t deadtimeInt;
    float deadtime;

    deadtimeInt = zynqAxiPwmDeadTimeRead(CUK_HW_CONFIG_PWM_BASE);
    deadtime = ( (float)deadtimeInt ) / ( (float)CUK_HW_PWM_CLK );

    return deadtime;
}
//-----------------------------------------------------------------------------
void cukHwSetAdcEnable(uint32_t enable){

    zynqAxiAdcEnableWrite(CUK_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcEnable(void){

    return zynqAxiAdcEnableRead(CUK_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetAdcManualTrigger(uint32_t trigger){

    zynqAxiAdcManualTriggerWrite(CUK_HW_CONFIG_ADC_BASE, trigger);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcManualTrigger(void){

    return zynqAxiAdcManualTriggerRead(CUK_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetAdcInterruptEnable(uint32_t enable){

    zynqAxiAdcInterruptEnableWrite(CUK_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcInterruptEnable(void){

    return zynqAxiAdcInterruptEnableRead(CUK_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetAdcSpiFreq(uint32_t freq){

    uint32_t clkdiv;

    clkdiv = CUK_HW_ADC_CLK / (freq << 1);

    zynqAxiAdcSpiClkDivWrite(CUK_HW_CONFIG_ADC_BASE, clkdiv);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcSpiFreq(void){

    uint32_t clkdiv, freq;

    clkdiv = zynqAxiAdcSpiClkDivRead(CUK_HW_CONFIG_ADC_BASE);

    freq = CUK_HW_ADC_CLK / (clkdiv << 1);

    return freq;
}
//-----------------------------------------------------------------------------
int32_t cukHwGetMeasurements(void *meas){

    cukConfigMeasurements_t *hwMeas;
    cukConfigSwMeasurements_t *swMeas;
    float **p;
    uint16_t *src;

    src = (uint16_t *)CUK_HW_CONFIG_ADC_BUFFER;
    p = (float **)meas;
    hwMeas = (cukConfigMeasurements_t *)p[0];
    swMeas = (cukConfigSwMeasurements_t *)p[1];

    //-------------------------------------------------------------------------
    // Sensor-based measurements
    //-------------------------------------------------------------------------
    /*
     * Skips the first adc channel of header. Each Cuk connector has five
     * measurements, but the ADC board has 6 channels. Thus, we skip the
     * first channel.
     */
    src++;
    hwMeas->ii      = hwControl.gains.ii_gain * ((float)(*src++)) + hwControl.gains.ii_ofs;
    hwMeas->i1      = hwControl.gains.i1_gain * ((float)(*src++)) + hwControl.gains.i1_ofs;

    hwMeas->vi      = hwControl.gains.vi_gain * ((float)(*src++)) + hwControl.gains.vi_ofs;
    hwMeas->vi_dc   = hwControl.gains.vi_dc_gain * ((float)(*src++)) + hwControl.gains.vi_dc_ofs;
    hwMeas->v1      = hwControl.gains.v1_gain * ((float)(*src++)) + hwControl.gains.v1_ofs;

    /* Skips the seventh adc channel of header */
    src++;
    hwMeas->io      = -( hwControl.gains.io_gain * ((float)(*src++)) + hwControl.gains.io_ofs );
    hwMeas->i2      = -( hwControl.gains.i2_gain * ((float)(*src++)) + hwControl.gains.i2_ofs );

    hwMeas->vo      = hwControl.gains.vo_gain * ((float)(*src++)) + hwControl.gains.vo_ofs;
    hwMeas->vo_dc   = hwControl.gains.vo_dc_gain * ((float)(*src++)) + hwControl.gains.vo_dc_ofs;
    hwMeas->v2      = hwControl.gains.v2_gain * ((float)(*src++)) + hwControl.gains.v2_ofs;
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Software-based measurements
    //-------------------------------------------------------------------------
    swMeas->ii_filt = dfiltExpMovAvg(hwMeas->ii, swMeas->ii_filt, hwControl.alpha);
    swMeas->io_filt = dfiltExpMovAvg(hwMeas->io, swMeas->io_filt, hwControl.alpha);

    swMeas->pi = hwMeas->i1 * hwMeas->vi_dc;
    swMeas->po = swMeas->io_filt * hwMeas->vo_dc;
    //-------------------------------------------------------------------------

    /* Protection */
    if( (hwMeas->ii > CUK_CONFIG_I_PRIM_LIM) || (hwMeas->i1 > CUK_CONFIG_I_PRIM_LIM) ) hwControl.status = 1;
    if( (hwMeas->ii < -CUK_CONFIG_I_PRIM_LIM) || (hwMeas->i1 < -CUK_CONFIG_I_PRIM_LIM) ) hwControl.status = 1;

    if( (hwMeas->vi > CUK_CONFIG_V_PRIM_LIM) || (hwMeas->vi_dc > CUK_CONFIG_V_PRIM_LIM) || (hwMeas->v1 > CUK_CONFIG_V_PRIM_LIM) ) hwControl.status = 1;

    if( (hwMeas->i2 > CUK_CONFIG_I_SEC_LIM) ) hwControl.status = 1;
    if( (hwMeas->i2 < -CUK_CONFIG_I_SEC_LIM) ) hwControl.status = 1;

    if( (hwMeas->vo > CUK_CONFIG_V_SEC_LIM) || (hwMeas->vo_dc > CUK_CONFIG_V_SEC_LIM) || (hwMeas->v2 > CUK_CONFIG_V_SEC_LIM) ) hwControl.status = 1;

    if( hwControl.status != 0 ){
        cukHwShutDown();
        return -1;
    }

    return sizeof(cukConfigMeasurements_t) + sizeof(cukConfigSwMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t cukHwApplyOutputs(void *outputs, int32_t size){

    cukConfigControl_t *control;

    control = (cukConfigControl_t *)outputs;

    cukHwSetPwmDuty(control->u);

    //control->u = 0.5f;

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwDisable(void){

    cukHwSetPwmDuty(0.0f);
    cukHwSetPwmOutputEnable(0);
    cukHwSetPwmOvfTriggerEnable(0);
}
//-----------------------------------------------------------------------------
void cukHwEnable(void){

    cukHwSetPwmDuty(0.0f);
    cukHwSetPwmOvfTriggerEnable(1);
    //cukHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
void cukHwControllerDisable(void){

    cukHwSetPwmOutputEnable(0);
}
//-----------------------------------------------------------------------------
void cukHwControllerEnable(void){

    cukHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
void cukHwSetLoadSwitch(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << CUK_HW_CONFIG_GPIO_LOAD_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL) & (~CUK_HW_CONFIG_GPIO_LOAD);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetLoadSwitch(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL) & (CUK_HW_CONFIG_GPIO_LOAD);

    gpio = gpio >> CUK_HW_CONFIG_GPIO_LOAD_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void cukHwSetOutputSwitch(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << CUK_HW_CONFIG_GPIO_OUTPUT_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL) & (~CUK_HW_CONFIG_GPIO_OUTPUT);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetOutputSwitch(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL) & (CUK_HW_CONFIG_GPIO_OUTPUT);

    gpio = gpio >> CUK_HW_CONFIG_GPIO_OUTPUT_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void cukHwSetMeasGains(cukConfigMeasGains_t *gains){

    hwControl.gains = *gains;
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetMeasGains(cukConfigMeasGains_t *gains){

    *gains = hwControl.gains;

    return sizeof(cukConfigMeasGains_t);
}
//-----------------------------------------------------------------------------
void cukHwSetFilterCoef(float alpha){

    hwControl.alpha = alpha;
}
//-----------------------------------------------------------------------------
float cukHwGetFilterCoef(void){

    return hwControl.alpha;
}
//-----------------------------------------------------------------------------
void cukHwShutDown(void){

    cukHwSetPwmOutputEnable(0);
    cukHwSetPwmDuty(0);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void cukHwInitializeAdc(void *intc, cukHwAdcIrqHandle_t irqhandle){

    uint32_t clkdiv;

    clkdiv = CUK_HW_ADC_CLK / (CUK_HW_CONFIG_ADC_SPI_FREQ_HZ << 1);

    zynqAxiAdcEnableWrite(CUK_HW_CONFIG_ADC_BASE, 0);

    zynqAxiAdcInterruptEnableWrite(CUK_HW_CONFIG_ADC_BASE, 1);

    zynqAxiAdcSpiClkDivWrite(CUK_HW_CONFIG_ADC_BASE, clkdiv);

    zynqAxiAdcBufferAddressWrite(CUK_HW_CONFIG_ADC_BASE, CUK_HW_CONFIG_ADC_BUFFER);

    zynqAxiAdcInterruptConfig(intc, CUK_HW_CONFIG_IRQ_PL_CPU1, CUK_HW_CONFIG_IRQ_PL_CPU1_PRIO, irqhandle);

    zynqAxiAdcEnableWrite(CUK_HW_CONFIG_ADC_BASE, 1);
}
//-----------------------------------------------------------------------------
static void cukHwInitializePwm(void){

    cukHwSetPwmReset(1);

    cukHwSetPwmFrequency(CUK_HW_CONFIG_PWM_FREQ_HZ);
    cukHwSetPwmDuty(0.0f);
    cukHwSetPwmDeadTime(CUK_HW_CONFIG_PWM_DEAD_TIME_NS);

    cukHwSetPwmOvfTriggerEnable(0);
    cukHwSetPwmOutputEnable(0);

    cukHwSetPwmReset(0);
}
//-----------------------------------------------------------------------------
static void cukHwInitializeGpio(void){

    /* Initializes GPIOs */
    XGpio_Initialize(&hwControl.gpio, CUK_HW_CONFIG_GPIO_BASE);
    XGpio_SetDataDirection(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL, 0);

    cukHwSetOutputSwitch(0);
    cukHwSetLoadSwitch(0);
}
//-----------------------------------------------------------------------------
static void cukHwInitializeMeasGains(void){

    hwControl.gains.ii_gain = 0.01246531060105208f;
    hwControl.gains.ii_ofs =  -25.036777021743315f;

    hwControl.gains.i1_gain = 0.025588979403530776f;
    hwControl.gains.i1_ofs =  -51.611508457879715f;

    hwControl.gains.vi_gain = 0.014919542630730998f;
    hwControl.gains.vi_ofs =  0.02983490778952813f;

    hwControl.gains.vi_dc_gain = 0.01490254584622419f;
    hwControl.gains.vi_dc_ofs =  0.014667383672769319f;

    hwControl.gains.v1_gain = 0.014919540229885057f;
    hwControl.gains.v1_ofs = 0.014919540229886508f;

    hwControl.gains.io_gain = 0.012489042423884562f;
    hwControl.gains.io_ofs =  -25.025032570046353f;

    hwControl.gains.i2_gain = 0.025757493479617852f;
    hwControl.gains.i2_ofs =  -52.08143710783329f;

    hwControl.gains.vo_gain = 0.01490621920998692f;
    hwControl.gains.vo_ofs =  0.021628758244869317f;

    hwControl.gains.vo_dc_gain = 0.014901654476279058f;
    hwControl.gains.vo_dc_ofs =  0.04172051494698792f;

    hwControl.gains.v2_gain = 0.01487120334913248f;
    hwControl.gains.v2_ofs =  0.058881268243716534f;
}
//-----------------------------------------------------------------------------
//=============================================================================
