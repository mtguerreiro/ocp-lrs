
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostHw.h"

#include "fsbuckboostConfig.h"

#include "stypes.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

static stypesMeasurements_t xtMeasurements;
static stypesSimData_t xtSimData;
static stypesControl_t xtControl;
static stypesControllerData_t xtControllerData;

typedef struct{

    uint32_t status;

    fsbuckboostConfigMeasurements_t meas;
    fsbuckboostConfigControl_t control;

    uint32_t pwmReset;
    uint32_t pwmOutputEnable;
    uint32_t pwmMode;
    uint32_t pwmFrequency;

}fsbuckboostHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static fsbuckboostHwControl_t hwControl = {
        .status = 0,

        .pwmReset = 0,
        .pwmOutputEnable = 0,
        .pwmMode = 0
};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwInitialize(fsbuckboostHwInitConfig_t *config){

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

    hwControl.pwmReset = reset;
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmReset(void){

    return hwControl.pwmReset;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmOutputEnable(uint32_t enable){

    hwControl.pwmOutputEnable = enable;
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmOutputEnable(void){

    return hwControl.pwmOutputEnable;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmOvfTriggerEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmOvfTriggerEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmInv(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmInv(void){

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmMode(uint32_t mode){

    xtControl.pwm_mode = mode;
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmMode(void){

    return xtControl.pwm_mode;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmLsSw(uint32_t state){
    
    xtControl.ls_sw = state;
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmLsSw(void){

    return xtControl.ls_sw;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmHsSw(uint32_t state){

    xtControl.hs_sw = state;
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmHsSw(void){

    return xtControl.hs_sw;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmFrequency(uint32_t freq){

    hwControl.pwmFrequency = freq;
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetPwmFrequency(void){

    return hwControl.pwmFrequency;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmDuty(float duty){

    xtControl.u = duty;
}
//-----------------------------------------------------------------------------
float fsbuckboostHwGetPwmDuty(void){

    return xtControl.u;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetPwmDeadTime(float deadtime){

}
//-----------------------------------------------------------------------------
float fsbuckboostHwGetPwmDeadTime(void){

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetAdcEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetAdcEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetAdcManualTrigger(uint32_t trigger){

}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetAdcManualTrigger(void){

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetAdcInterruptEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetAdcInterruptEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetAdcSpiFreq(uint32_t freq){

}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetAdcSpiFreq(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwGetMeasurements(void *meas){

    fsbuckboostConfigMeasurements_t *dst;

    dst = (fsbuckboostConfigMeasurements_t *)meas;

    /* Measurements */
    dst->ii = xtMeasurements.ii;
    dst->v_in = xtMeasurements.v_in;
    dst->il = xtMeasurements.il;
    dst->v_out = xtMeasurements.v_out;
    dst->io = xtMeasurements.io;
    dst->v_dc_out = xtMeasurements.v_dc_out;

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

    return fsbuckboostHwOpilUpdateControl(outputs, size);
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

    xtControl.input_relay = state;
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetInputRelay(void){

    return xtControl.input_relay;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetOutputRelay(uint32_t state){

    xtControl.output_relay = state;
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetOutputRelay(void){

    return xtControl.output_relay;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetLoadSwitch(uint32_t state){

    xtControl.load_sw = state;
}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetLoadSwitch(void){

    return xtControl.load_sw;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwSetMeasGains(fsbuckboostConfigMeasGains_t *gains){

}
//-----------------------------------------------------------------------------
uint32_t fsbuckboostHwGetMeasGains(fsbuckboostConfigMeasGains_t *gains){

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostHwShutDown(void){

    fsbuckboostHwSetPwmOutputEnable(0);
    fsbuckboostHwSetLoadSwitch(0);
    fsbuckboostHwSetOutputRelay(0);
    fsbuckboostHwSetInputRelay(0);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwOpilUpdateMeasurements(void *meas, int32_t size){

    uint8_t *src, *dst;

    dst = (uint8_t *)( &xtMeasurements );
    src = (uint8_t *)( meas );

    while(size--) *dst++ = *src++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwOpilUpdateSimData(void *simData, int32_t size){

    uint8_t *src, *dst;

    dst = (uint8_t *)( &xtSimData );
    src = (uint8_t *)( simData );

    while(size--) *dst++ = *src++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwOpilGetSimData(void **simData, int32_t size){

    *simData = (void *)( &xtSimData );

    return sizeof(stypesSimData_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwOpilUpdateControl(void *control, int32_t size){

    uint8_t *src, *dst;
    int32_t n;

    dst = (uint8_t *)( &xtControl );
    src = (uint8_t *)( control );

    n = size;
    while(n--) *dst++ = *src++;

    return size;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwOpilGetControl(void **control){

    *control = (void *)( &xtControl );

    return sizeof(stypesControl_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostHwOpilUpdateControllerData(void *data, int32_t size){

    uint8_t *src, *dst;

    dst = (uint8_t *)( &xtControllerData );
    src = (uint8_t *)( data );

    while(size--) *dst++ = *src++;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostHwOpilGetControllerData(void **controllerData){

    *controllerData = (void *)( &xtControllerData );

    return sizeof(stypesControllerData_t);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================

//-----------------------------------------------------------------------------
//=============================================================================

