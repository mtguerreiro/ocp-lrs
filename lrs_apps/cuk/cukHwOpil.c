
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukHwOpil.h"

#include "cukConfig.h"

#include "stypes.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    uint32_t status;

    float alpha;

    uint32_t pwmFrequency;

}cukHwControl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static stypesMeasurements_t xtMeasurements;
static stypesSimData_t xtSimData;
static stypesControl_t xtControl;
static stypesControllerData_t xtControllerData;

static cukHwControl_t hwControl = {.status = 0, .alpha = 0.2f};
static float i_i_filt = 0.0f, i_1_filt = 0.0f, i_o_filt = 0.0f, i_2_filt = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukHwInitialize(void){

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

}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmReset(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmOutputEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmOutputEnable(void){

    return 0;
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

    int32_t meassize;

    cukConfigMeasurements_t *dst;

    dst = (cukConfigMeasurements_t *)meas;

    dst->ii = xtMeasurements.ii;
    dst->i1 = xtMeasurements.i1;

    dst->vi = xtMeasurements.vi;
    dst->vi_dc = xtMeasurements.vi_dc;
    dst->v1 = xtMeasurements.v1;

    dst->io = xtMeasurements.io;
    dst->i2 = xtMeasurements.i2;

    dst->vo = xtMeasurements.vo;
    dst->vo_dc = xtMeasurements.vo_dc;
    dst->v2 = xtMeasurements.v2;

    dst->ii_filt = 0.0f;
    dst->i1_filt = 0.0f;

    dst->vi_filt = 0.0f;
    dst->vi_dc_filt = 0.0f;
    dst->v1_filt = 0.0f;

    dst->io_filt = 0.0f;
    dst->i2_filt = 0.0f;

    dst->vo_filt = 0.0f;
    dst->vo_dc_filt = 0.0f;
    dst->v2_filt = 0.0f;

    dst->p_load = xtMeasurements.p_load;

    i_1_filt = cukHwExpMovAvg(dst->i1, i_1_filt);
    dst->i1_filt = i_1_filt;

    i_i_filt = cukHwExpMovAvg(dst->ii, i_i_filt);
    dst->ii_filt = i_i_filt;

    i_o_filt = cukHwExpMovAvg(dst->io, i_o_filt);
    dst->io_filt = i_o_filt;

    i_2_filt = cukHwExpMovAvg(dst->i2, i_2_filt);
    dst->i2_filt = i_2_filt;

    dst->pi = dst->i1 * dst->vi_dc;
    dst->po = i_o_filt * dst->vo_dc;

    /* Protection */
    if( (dst->ii > CUK_CONFIG_I_PRIM_LIM) || (dst->i1 > CUK_CONFIG_I_PRIM_LIM) ) hwControl.status = 1;
    if( (dst->ii < -CUK_CONFIG_I_PRIM_LIM) || (dst->i1 < -CUK_CONFIG_I_PRIM_LIM) ) hwControl.status = 1;

    if( (dst->vi > CUK_CONFIG_V_PRIM_LIM) || (dst->vi_dc > CUK_CONFIG_V_PRIM_LIM) || (dst->v1 > CUK_CONFIG_V_PRIM_LIM) ) hwControl.status = 1;

    if( (dst->i2 > CUK_CONFIG_I_SEC_LIM) ) hwControl.status = 1;
    if( (dst->i2 < -CUK_CONFIG_I_SEC_LIM) ) hwControl.status = 1;

    if( (dst->vo > CUK_CONFIG_V_SEC_LIM) || (dst->vo_dc > CUK_CONFIG_V_SEC_LIM) || (dst->v2 > CUK_CONFIG_V_SEC_LIM) ) hwControl.status = 1;

    if( hwControl.status != 0 ){
        //cukHwSetPwmOutputEnable(0);
        cukHwShutDown();
        return -1;
    }
    else
        return sizeof(cukConfigMeasurements_t);

    return sizeof(cukConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t cukHwApplyOutputs(void *outputs, int32_t size){

    return cukHwOpilUpdateControl(outputs, size);
}
//-----------------------------------------------------------------------------
void cukHwDisable(void){

    cukHwSetPwmDuty(0.0f);
}
//-----------------------------------------------------------------------------
void cukHwEnable(void){

}
//-----------------------------------------------------------------------------
void cukHwControllerDisable(void){

}
//-----------------------------------------------------------------------------
void cukHwControllerEnable(void){

}
//-----------------------------------------------------------------------------
void cukHwSetLoadSwitch(uint32_t state){

    xtControllerData.load_sw = state;
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetLoadSwitch(void){

    return ( (uint32_t)xtControllerData.load_sw );
}
//-----------------------------------------------------------------------------
void cukHwSetOutputSwitch(uint32_t state){

    xtControllerData.output_sw = state;
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetOutputSwitch(void){

    return ( (uint32_t)xtControllerData.output_sw );
}
//-----------------------------------------------------------------------------
void cukHwSetMeasGains(cukConfigMeasGains_t *gains){

}
//-----------------------------------------------------------------------------
uint32_t cukHwGetMeasGains(cukConfigMeasGains_t *gains){

    return 0;
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
float cukHwExpMovAvg(float sample, float average){

    return hwControl.alpha * sample + (1.0f - hwControl.alpha) * average;
}
//-----------------------------------------------------------------------------
void cukHwShutDown(void){

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

