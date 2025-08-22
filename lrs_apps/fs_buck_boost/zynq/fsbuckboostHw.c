
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostHw.h"

#include "xparameters.h"
#include "zynqAxiFsPwm.h"
#include "zynqAxiAdc.h"

#include "fsbuckboostConfig.h"

#include "zynqConfig.h"

#include "xgpio.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define FS_BUCK_BOOST_HW_CONFIG_ADC_SPI_FREQ_HZ      ((uint32_t)10000000)
#define FS_BUCK_BOOST_HW_CONFIG_PWM_FREQ_HZ          ((uint32_t) 100000 )
#define FS_BUCK_BOOST_HW_CONFIG_PWM_DEAD_TIME_NS     ((float) 200e-9 )
#define FS_BUCK_BOOST_HW_CONFIG_PWM_BASE             XPAR_AXI_PWM_BUCKBOOST_0_BASEADDR
#define FS_BUCK_BOOST_HW_CONFIG_ADC_BASE             XPAR_ADC_PSCTL_0_BASEADDR

#define FS_BUCK_BOOST_HW_CONFIG_IRQ_PL_CPU1          ZYNQ_CONFIG_IRQ_PL_TO_CPU1
#define FS_BUCK_BOOST_HW_CONFIG_IRQ_PL_CPU1_PRIO     ZYNQ_CONFIG_IRQ_PL_TO_CPU1_PRIO
#define FS_BUCK_BOOST_HW_CONFIG_ADC_BUFFER           ( ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR )

#define FS_BUCK_BOOST_HW_CONFIG_GPIO_BASE            XPAR_XGPIO_0_BASEADDR
#define FS_BUCK_BOOST_HW_CONFIG_GPIO_CHANNEL         1

#define FS_BUCK_BOOST_HW_CONFIG_GPIO_IN_RLY_OFFS     (1U)
#define FS_BUCK_BOOST_HW_CONFIG_GPIO_IN_RLY          (1 << FS_BUCK_BOOST_HW_CONFIG_GPIO_IN_RLY_OFFS)
#define FS_BUCK_BOOST_HW_CONFIG_GPIO_OUT_RLY_OFFS    (0U)
#define FS_BUCK_BOOST_HW_CONFIG_GPIO_OUT_RLY         (1 << FS_BUCK_BOOST_HW_CONFIG_GPIO_OUT_RLY_OFFS)
//#define FS_BUCK_BOOST_HW_CONFIG_GPIO_LOAD_SW_OFFS    (1U)
//#define FS_BUCK_BOOST_HW_CONFIG_GPIO_LOAD_SW         (1 << FS_BUCK_BOOST_HW_CONFIG_GPIO_LOAD_SW_OFFS)

/* PWM peripheral clock, in Hz */
#define FS_BUCK_BOOST_HW_PWM_CLK                      100000000
#define FS_BUCK_BOOST_HW_ADC_CLK                      100000000

typedef struct{

    uint32_t status;

    uint32_t pwmPeriod;

    fsbuckboostConfigMeasurements_t meas;
    fsbuckboostConfigControl_t control;

    XGpio gpio;

    fsbuckboostConfigMeasGains_t gains;

    float alpha;

}fsbuckboostHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void fsbuckboostHwInitializeAdc(void *intc, fsbuckboostHwAdcIrqHandle_t irqhandle);
static void fsbuckboostHwInitializePwm(void);
static void fsbuckboostHwInitializeGpio(void);
static void fsbuckboostHwInitializeMeasGains(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static fsbuckboostHwControl_t hwControl = {.pwmPeriod = 0, .status = 0, .alpha = 0.2f};
static float i_i_filt = 0.0f, i_1_filt = 0.0f, i_o_filt = 0.0f, i_2_filt = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwInitialize(fsbuckboostHwInitConfig_t *config){

    fsbuckboostHwInitializeAdc(config->intc, config->irqhandle);
    fsbuckboostHwInitializePwm();
    fsbuckboostHwInitializeGpio();
    fsbuckboostHwInitializeMeasGains();

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwStatus(void){

    return hwControl.status;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwStatusClear(void){

    hwControl.status = 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmReset(uint32_t reset){

    zynqAxiFsPwmResetWrite(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE, reset);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmReset(void){

    return zynqAxiFsPwmResetRead(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmOutputEnable(uint32_t enable){

    zynqAxiFsPwmOutputEnableWrite(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmOutputEnable(void){

    return zynqAxiFsPwmOutputEnableRead(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmOvfTriggerEnable(uint32_t enable){

    zynqAxiFsPwmOvfTriggerEnableWrite(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmOvfTriggerEnable(void){

    return zynqAxiFsPwmOvfTriggerEnableRead(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmInv(uint32_t enable){

    zynqAxiFsPwmInvWrite(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmInv(void){

    return zynqAxiFsPwmInvRead(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmMode(uint32_t mode){

    zynqAxiFsPwmModeWrite(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE, mode);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmMode(void){

    return zynqAxiFsPwmModeRead(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmLsSw(uint32_t state){

    zynqAxiFsPwmLsSwWrite(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE, state);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmLsSw(void){

    return zynqAxiFsPwmLsSwRead(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmHsSw(uint32_t state){

    zynqAxiFsPwmHsSwWrite(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE, state);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmHsSw(void){

    return zynqAxiFsPwmHsSwRead(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmFrequency(uint32_t freq){

    uint32_t period;

    period = FS_BUCK_BOOST_HW_PWM_CLK / (freq << 1);

    hwControl.pwmPeriod = period;

    zynqAxiFsPwmPeriodWrite(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE, period);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmFrequency(void){

    uint32_t freq, period;

    period = zynqAxiFsPwmPeriodRead(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE);

    freq = FS_BUCK_BOOST_HW_PWM_CLK / (period << 1);

    return freq;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmDuty(float duty){

    uint32_t dutyInt;

    dutyInt = (uint32_t)( duty * ((float)hwControl.pwmPeriod ) );

    zynqAxiFsPwmDutyWrite(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE, dutyInt);
}
//-----------------------------------------------------------------------------
float fsbuckboostHwGetPwmDuty(void){

    uint32_t dutyInt;
    float duty;

    dutyInt = zynqAxiFsPwmDutyRead(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE);
    duty = ( (float)dutyInt ) / ( (float)hwControl.pwmPeriod );

    return duty;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmDeadTime(float deadtime){

    uint32_t deadtimeInt;

    deadtimeInt = (uint32_t)( deadtime * ((float)FS_BUCK_BOOST_HW_PWM_CLK) );

    zynqAxiFsPwmDeadTimeWrite(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE, deadtimeInt);
}
//-----------------------------------------------------------------------------
float fsbuckboostHwGetPwmDeadTime(void){

    uint32_t deadtimeInt;
    float deadtime;

    deadtimeInt = zynqAxiFsPwmDeadTimeRead(FS_BUCK_BOOST_HW_CONFIG_PWM_BASE);
    deadtime = ( (float)deadtimeInt ) / ( (float)FS_BUCK_BOOST_HW_PWM_CLK );

    return deadtime;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetAdcEnable(uint32_t enable){

    zynqAxiAdcEnableWrite(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetAdcEnable(void){

    return zynqAxiAdcEnableRead(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetAdcManualTrigger(uint32_t trigger){

    zynqAxiAdcManualTriggerWrite(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE, trigger);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetAdcManualTrigger(void){

    return zynqAxiAdcManualTriggerRead(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetAdcInterruptEnable(uint32_t enable){

    zynqAxiAdcInterruptEnableWrite(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetAdcInterruptEnable(void){

    return zynqAxiAdcInterruptEnableRead(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetAdcSpiFreq(uint32_t freq){

    uint32_t clkdiv;

    clkdiv = FS_BUCK_BOOST_HW_ADC_CLK / (freq << 1);

    zynqAxiAdcSpiClkDivWrite(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE, clkdiv);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetAdcSpiFreq(void){

    uint32_t clkdiv, freq;

    clkdiv = zynqAxiAdcSpiClkDivRead(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE);

    freq = FS_BUCK_BOOST_HW_ADC_CLK / (clkdiv << 1);

    return freq;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwGetMeasurements(void *meas){

    fsbuckboostConfigMeasurements_t *dst;
    uint16_t *src;

    src = (uint16_t *)FS_BUCK_BOOST_HW_CONFIG_ADC_BUFFER;
    dst = (fsbuckboostConfigMeasurements_t *)meas;

    /* Measurements */
    dst->ii = hwControl.gains.ii_gain * ((float)(*src++)) + hwControl.gains.ii_ofs;
    dst->v_in = hwControl.gains.v_in_gain * ((float)(*src++)) + hwControl.gains.v_in_ofs;
    dst->il = hwControl.gains.il_gain * ((float)(*src++)) + hwControl.gains.il_ofs;
    dst->v_out = hwControl.gains.v_out_gain * ((float)(*src++)) + hwControl.gains.v_out_ofs;
    dst->io = hwControl.gains.io_gain * ((float)(*src++)) + hwControl.gains.io_ofs;
    dst->v_dc_out = hwControl.gains.v_dc_out_gain * ((float)(*src++)) + hwControl.gains.v_dc_out_ofs;

    /* Protection */
    if( (dst->ii > FS_BUCK_BOOST_CONFIG_I_LIM) || (dst->il > FS_BUCK_BOOST_CONFIG_I_LIM) )
        hwControl.status = 1;

    if( (dst->ii < -FS_BUCK_BOOST_CONFIG_I_LIM) || (dst->il < -FS_BUCK_BOOST_CONFIG_I_LIM) )
        hwControl.status = 1;

    if( (dst->v_in > FS_BUCK_BOOST_CONFIG_V_LIM) || (dst->v_dc_out > FS_BUCK_BOOST_CONFIG_V_LIM) || (dst->v_out > FS_BUCK_BOOST_CONFIG_V_LIM) )
        hwControl.status = 1;

    if( hwControl.status != 0 ){
        fsbuckboostHwShutDown();
        return -1;
    }

    return sizeof(fsbuckboostConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwApplyOutputs(void *outputs, int32_t size){

    (void)size;

    fsbuckboostConfigControl_t *control;

    control = (fsbuckboostConfigControl_t *)outputs;

    fsbuckboostHwSetPwmDuty(control->u);

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwDisable(void){

    fsbuckboostHwSetPwmDuty(0.0f);
    fsbuckboostHwSetPwmOutputEnable(0);
    fsbuckboostHwSetPwmOvfTriggerEnable(0);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwEnable(void){

    fsbuckboostHwSetPwmDuty(0.0f);
    fsbuckboostHwSetPwmOvfTriggerEnable(1);
    //fsbuckboostHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwControllerDisable(void){

    fsbuckboostHwSetPwmOutputEnable(0);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwControllerEnable(void){

    fsbuckboostHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetInputRelay(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << FS_BUCK_BOOST_HW_CONFIG_GPIO_IN_RLY_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, FS_BUCK_BOOST_HW_CONFIG_GPIO_CHANNEL) & (~FS_BUCK_BOOST_HW_CONFIG_GPIO_IN_RLY);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, FS_BUCK_BOOST_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetInputRelay(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, FS_BUCK_BOOST_HW_CONFIG_GPIO_CHANNEL) & (FS_BUCK_BOOST_HW_CONFIG_GPIO_IN_RLY);

    gpio = gpio >> FS_BUCK_BOOST_HW_CONFIG_GPIO_IN_RLY_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetOutputRelay(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << FS_BUCK_BOOST_HW_CONFIG_GPIO_OUT_RLY_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, FS_BUCK_BOOST_HW_CONFIG_GPIO_CHANNEL) & (~FS_BUCK_BOOST_HW_CONFIG_GPIO_OUT_RLY);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, FS_BUCK_BOOST_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetOutputRelay(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, FS_BUCK_BOOST_HW_CONFIG_GPIO_CHANNEL) & (FS_BUCK_BOOST_HW_CONFIG_GPIO_OUT_RLY);

    gpio = gpio >> FS_BUCK_BOOST_HW_CONFIG_GPIO_OUT_RLY_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetLoadSwitch(uint32_t state){

//    uint32_t gpio;
//
//    state = (state & 0x01) << FS_BUCK_BOOST_HW_CONFIG_GPIO_LOAD_SW_OFFS;
//
//    gpio = XGpio_DiscreteRead(&hwControl.gpio, FS_BUCK_BOOST_HW_CONFIG_GPIO_CHANNEL) & (~FS_BUCK_BOOST_HW_CONFIG_GPIO_LOAD_SW);
//
//    gpio = gpio | state;
//
//    XGpio_DiscreteWrite(&hwControl.gpio, FS_BUCK_BOOST_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetLoadSwitch(void){

//    uint32_t gpio;
//
//    gpio = XGpio_DiscreteRead(&hwControl.gpio, FS_BUCK_BOOST_HW_CONFIG_GPIO_CHANNEL) & (FS_BUCK_BOOST_HW_CONFIG_GPIO_LOAD_SW);
//
//    gpio = gpio >> FS_BUCK_BOOST_HW_CONFIG_GPIO_LOAD_SW_OFFS;
//
//    return gpio;
    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetMeasGains(fsbuckboostConfigMeasGains_t *gains){

    hwControl.gains = *gains;
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetMeasGains(fsbuckboostConfigMeasGains_t *gains){

    *gains = hwControl.gains;

    return sizeof(fsbuckboostConfigMeasGains_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwShutDown(void){

    fsbuckboostHwSetPwmOutputEnable(0);
    fsbuckboostHwSetInputRelay(0);
    fsbuckboostHwSetOutputRelay(0);
    fsbuckboostHwSetLoadSwitch(0);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void fsbuckboostHwInitializeAdc(void *intc, fsbuckboostHwAdcIrqHandle_t irqhandle){

    uint32_t clkdiv;

    clkdiv = FS_BUCK_BOOST_HW_ADC_CLK / (FS_BUCK_BOOST_HW_CONFIG_ADC_SPI_FREQ_HZ << 1);

    zynqAxiAdcEnableWrite(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE, 0);

    zynqAxiAdcInterruptEnableWrite(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE, 1);

    zynqAxiAdcSpiClkDivWrite(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE, clkdiv);

    zynqAxiAdcBufferAddressWrite(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE, FS_BUCK_BOOST_HW_CONFIG_ADC_BUFFER);

    zynqAxiAdcInterruptConfig(intc, FS_BUCK_BOOST_HW_CONFIG_IRQ_PL_CPU1, FS_BUCK_BOOST_HW_CONFIG_IRQ_PL_CPU1_PRIO, irqhandle);

    zynqAxiAdcEnableWrite(FS_BUCK_BOOST_HW_CONFIG_ADC_BASE, 1);
}
//-----------------------------------------------------------------------------
static void fsbuckboostHwInitializePwm(void){

    fsbuckboostHwSetPwmReset(1);

    fsbuckboostHwSetPwmFrequency(FS_BUCK_BOOST_HW_CONFIG_PWM_FREQ_HZ);
    fsbuckboostHwSetPwmDuty(0.0f);
    fsbuckboostHwSetPwmDeadTime(FS_BUCK_BOOST_HW_CONFIG_PWM_DEAD_TIME_NS);

    fsbuckboostHwSetPwmOvfTriggerEnable(0);
    fsbuckboostHwSetPwmOutputEnable(0);

    fsbuckboostHwSetPwmMode(0);
    fsbuckboostHwSetPwmHsSw(0);
    fsbuckboostHwSetPwmLsSw(0);

    fsbuckboostHwSetPwmReset(0);
}
//-----------------------------------------------------------------------------
static void fsbuckboostHwInitializeGpio(void){

    /* Initializes GPIOs */
    XGpio_Initialize(&hwControl.gpio, FS_BUCK_BOOST_HW_CONFIG_GPIO_BASE);
    XGpio_SetDataDirection(&hwControl.gpio, FS_BUCK_BOOST_HW_CONFIG_GPIO_CHANNEL, 0);

    fsbuckboostHwSetInputRelay(0);
    fsbuckboostHwSetOutputRelay(0);
    fsbuckboostHwSetLoadSwitch(0);
}
//-----------------------------------------------------------------------------
static void fsbuckboostHwInitializeMeasGains(void){

    hwControl.gains.ii_gain = FS_BUCK_BOOST_CFG_II_GAIN;
    hwControl.gains.ii_ofs =  FS_BUCK_BOOST_CFG_II_OFFS;

    hwControl.gains.il_gain = FS_BUCK_BOOST_CFG_IL_GAIN;
    hwControl.gains.il_ofs =  FS_BUCK_BOOST_CFG_IL_OFFS;

    hwControl.gains.io_gain = FS_BUCK_BOOST_CFG_IO_GAIN;
    hwControl.gains.io_ofs =  FS_BUCK_BOOST_CFG_IO_OFFS;

    hwControl.gains.v_in_gain = FS_BUCK_BOOST_CFG_V_IN_GAIN;
    hwControl.gains.v_in_ofs =  FS_BUCK_BOOST_CFG_V_IN_OFFS;

    hwControl.gains.v_dc_out_gain = FS_BUCK_BOOST_CFG_V_DC_OUT_GAIN;
    hwControl.gains.v_dc_out_ofs =  FS_BUCK_BOOST_CFG_V_DC_OUT_OFFS;

    hwControl.gains.v_out_gain = FS_BUCK_BOOST_CFG_V_OUT_GAIN;
    hwControl.gains.v_out_ofs = FS_BUCK_BOOST_CFG_V_OUT_OFFS;
}
//-----------------------------------------------------------------------------
//=============================================================================
