
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukHw.h"

#include "cukConfig.h"

#include "stypes.h"

#include "utils/dfilt.h"
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

    cukConfigMeasurements_t meas;
    cukConfigControl_t control;
    cukConfigMeasGains_t gains;

    uint32_t pwmReset;
    uint32_t pwmOutputEnable;
    uint32_t pwmFrequency;

    float alpha;
}cukHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static cukHwControl_t hwControl = {
    .status = 0,
    .alpha = 0.2f
};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukHwInitialize(cukHwInitConfig_t *config){


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

    hwControl.pwmReset = reset;
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmReset(void){

    return hwControl.pwmReset;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmOutputEnable(uint32_t enable){

    hwControl.pwmOutputEnable = enable;
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmOutputEnable(void){

    return hwControl.pwmOutputEnable;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmOvfTriggerEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmOvfTriggerEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmFrequency(uint32_t freq){

    hwControl.pwmFrequency = freq;
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmFrequency(void){

    return hwControl.pwmFrequency;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmDuty(float duty){

    xtControl.u = duty;
}
//-----------------------------------------------------------------------------
float cukHwGetPwmDuty(void){

    return xtControl.u;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmDeadTime(float deadtime){

}
//-----------------------------------------------------------------------------
float cukHwGetPwmDeadTime(void){

    return 0.0f;
}
//-----------------------------------------------------------------------------
void cukHwSetAdcEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwSetAdcManualTrigger(uint32_t trigger){

}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcManualTrigger(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwSetAdcInterruptEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcInterruptEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwSetAdcSpiFreq(uint32_t freq){

}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcSpiFreq(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukHwGetMeasurements(void *meas){

    cukConfigMeasurements_t *hwMeas;
    cukConfigSwMeasurements_t *softMeas;
    float **p;

    p = (float **)meas;

    hwMeas = (cukConfigMeasurements_t *)p[0];
    softMeas = (cukConfigSwMeasurements_t *)p[1];

    /* Sensor-based measurements */
    hwMeas->ii      = xtMeasurements.ii;
    hwMeas->i1      = xtMeasurements.i1;

    hwMeas->vi      = xtMeasurements.vi;
    hwMeas->vi_dc   = xtMeasurements.vi_dc;
    hwMeas->v1      = xtMeasurements.v1;

    hwMeas->io      = xtMeasurements.io;
    hwMeas->i2      = xtMeasurements.i2;

    hwMeas->vo      = xtMeasurements.vo;
    hwMeas->vo_dc   = xtMeasurements.vo_dc;
    hwMeas->v2      = xtMeasurements.v2;

    /* Software-based measurements */
    softMeas->ii_filt = dfiltExpMovAvg(hwMeas->ii, softMeas->ii_filt, hwControl.alpha);
    softMeas->io_filt = dfiltExpMovAvg(hwMeas->io, softMeas->io_filt, hwControl.alpha);

    softMeas->pi = hwMeas->i1 * hwMeas->vi_dc;
    softMeas->po = softMeas->io_filt * hwMeas->vo_dc;

    /* Protection */
    if( (hwMeas->ii > CUK_CONFIG_I_PRIM_LIM) || (hwMeas->i1 > CUK_CONFIG_I_PRIM_LIM) )
        hwControl.status = 1;
    if( (hwMeas->ii < -CUK_CONFIG_I_PRIM_LIM) || (hwMeas->i1 < -CUK_CONFIG_I_PRIM_LIM) )
        hwControl.status = 1;

    if( (hwMeas->vi_dc > CUK_CONFIG_V_PRIM_LIM) || (hwMeas->v1 > CUK_CONFIG_V_PRIM_LIM) )
        hwControl.status = 1;

    if( (hwMeas->i2 > CUK_CONFIG_I_SEC_LIM) ) hwControl.status = 1;
    if( (hwMeas->i2 < -CUK_CONFIG_I_SEC_LIM) ) hwControl.status = 1;

    if( (hwMeas->vo_dc > CUK_CONFIG_V_SEC_LIM) || (hwMeas->v2 > CUK_CONFIG_V_SEC_LIM) )
        hwControl.status = 1;

    if( hwControl.status != 0 ){
        cukHwShutDown();
        return -1;
    }
    else
        return sizeof(cukConfigMeasurements_t) + sizeof(cukConfigSwMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t cukHwApplyOutputs(void *outputs, int32_t size){

    return cukHwOpilUpdateControl(outputs, size);
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

    xtControllerData.load_sw = state;
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetLoadSwitch(void){

    return xtControllerData.load_sw;
}
//-----------------------------------------------------------------------------
void cukHwSetOutputSwitch(uint32_t state){

    xtControllerData.output_sw = state;
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetOutputSwitch(void){

    return xtControllerData.output_sw;
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
int32_t cukHwOpilUpdateMeasurements(void *meas, int32_t size){

    uint8_t *src, *dst;

    dst = (uint8_t *)( &xtMeasurements );
    src = (uint8_t *)( meas );

    while(size--) *dst++ = *src++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukHwOpilUpdateSimData(void *simData, int32_t size){

    uint8_t *src, *dst;

    dst = (uint8_t *)( &xtSimData );
    src = (uint8_t *)( simData );

    while(size--) *dst++ = *src++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukHwOpilGetSimData(void **simData, int32_t size){

    *simData = (void *)( &xtSimData );

    return sizeof(stypesSimData_t);
}
//-----------------------------------------------------------------------------
int32_t cukHwOpilUpdateControl(void *control, int32_t size){

    uint8_t *src, *dst;
    int32_t n;

    dst = (uint8_t *)( &xtControl );
    src = (uint8_t *)( control );

    n = size;
    while(n--) *dst++ = *src++;

    return size;
}
//-----------------------------------------------------------------------------
int32_t cukHwOpilGetControl(void **control){

    *control = (void *)( &xtControl );

    return sizeof(stypesControl_t);
}
//-----------------------------------------------------------------------------
void cukHwOpilUpdateControllerData(void *data, int32_t size){

    uint8_t *src, *dst;

    dst = (uint8_t *)( &xtControllerData );
    src = (uint8_t *)( data );

    while(size--) *dst++ = *src++;
}
//-----------------------------------------------------------------------------
int32_t cukHwOpilGetControllerData(void **controllerData){

    *controllerData = (void *)( &xtControllerData );

    return sizeof(stypesControllerData_t);
}
//-----------------------------------------------------------------------------
//=============================================================================
