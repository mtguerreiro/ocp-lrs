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
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

typedef struct{

    uint32_t status;

    uint32_t pwmPeriod;

    buckConfigMeasurements_t meas;
    buckConfigControl_t control;

    buckConfigMeasGains_t gains;

    float alpha;

}buckHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void buckHwInitializeAdc(void);
static void buckHwInitializePwm(void);
static void buckHwInitializeGpio(void);
static void buckHwInitializeMeasGains(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static buckHwControl_t hwControl = {.pwmPeriod = 0, .status = 0, .alpha = 0.2f};
static float i_i_filt = 0.0f, i_1_filt = 0.0f, i_o_filt = 0.0f, i_2_filt = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckHwInit(void *param){

    (void)param;

    buckHwInitializeAdc();
    buckHwInitializePwm();
    buckHwInitializeGpio();
    buckHwInitializeMeasGains();

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckHwStatus(void){

    return hwControl.status;
}
//-----------------------------------------------------------------------------
void buckHwStatusClear(void){

    hwControl.status = 0;
}
//-----------------------------------------------------------------------------
void buckHwSetPwmEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t buckHwGetPwmEnable(void){

    uint32_t enable = 0;

    return enable;
}

//-----------------------------------------------------------------------------
void buckHwSetPwmFrequency(uint32_t freq){

}
//-----------------------------------------------------------------------------
uint32_t buckHwGetPwmFrequency(void){

    uint32_t freq = 0;

    return freq;
}
//-----------------------------------------------------------------------------
void buckHwSetPwmDuty(float duty){

    uint32_t dutyInt;

    dutyInt = (uint32_t)( duty * ((float)hwControl.pwmPeriod ) );
}
//-----------------------------------------------------------------------------
float buckHwGetPwmDuty(void){

    uint32_t dutyInt = 0;
    float duty = 0.0f;

    return duty;
}
//-----------------------------------------------------------------------------
void buckHwSetPwmDeadTime(float deadtime){

    uint32_t deadtimeInt = 0;
}
//-----------------------------------------------------------------------------
float buckHwGetPwmDeadTime(void){

    float deadtime = 0.0f;

    return deadtime;
}
//-----------------------------------------------------------------------------
int32_t buckHwGetMeasurements(void *meas){


    return sizeof(buckConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t buckHwApplyOutputs(void *outputs, int32_t size){

    buckConfigControl_t *control;

    control = (buckConfigControl_t *)outputs;

    buckHwSetPwmDuty(control->u);

    return 0;
}
//-----------------------------------------------------------------------------
void buckHwDisable(void){

}
//-----------------------------------------------------------------------------
void buckHwEnable(void){

}
//-----------------------------------------------------------------------------
void buckHwControllerDisable(void){

}
//-----------------------------------------------------------------------------
void buckHwControllerEnable(void){

}
//-----------------------------------------------------------------------------
void buckHwSetInputRelay(uint32_t state){

}
//-----------------------------------------------------------------------------
uint32_t buckHwGetInputRelay(void){

    uint32_t gpio = 0;

    return gpio;
}
//-----------------------------------------------------------------------------
void buckHwSetOutputRelay(uint32_t state){

}
//-----------------------------------------------------------------------------
uint32_t buckHwGetOutputRelay(void){

    uint32_t gpio = 0;;
}
//-----------------------------------------------------------------------------
void buckHwSetMeasGains(buckConfigMeasGains_t *gains){

    hwControl.gains = *gains;
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetMeasGains(buckConfigMeasGains_t *gains){

    *gains = hwControl.gains;

    return sizeof(buckConfigMeasGains_t);
}
//-----------------------------------------------------------------------------
void buckHwShutDown(void){

}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void buckHwInitializeAdc(void){

}
//-----------------------------------------------------------------------------
static void buckHwInitializePwm(void){

}
//-----------------------------------------------------------------------------
static void buckHwInitializeGpio(void){

}
//-----------------------------------------------------------------------------
static void buckHwInitializeMeasGains(void){


    hwControl.gains.io_gain = BUCK_CONFIG_IO_AVG_GAIN;
    hwControl.gains.io_ofs =  BUCK_CONFIG_IO_AVG_OFFS;

    hwControl.gains.il_gain = BUCK_CONFIG_IL_GAIN;
    hwControl.gains.il_ofs =  BUCK_CONFIG_IL_OFFS;
   
    hwControl.gains.v_dc_out_gain = BUCK_CONFIG_V_DC_OUT_GAIN;
    hwControl.gains.v_dc_out_ofs =  BUCK_CONFIG_V_DC_OUT_OFFS;

    hwControl.gains.v_out_gain = BUCK_CONFIG_V_OUT_GAIN;
    hwControl.gains.v_out_ofs = BUCK_CONFIG_V_OUT_OFFS;
    
    hwControl.gains.v_dc_in_gain = BUCK_CONFIG_V_DC_IN_GAIN;
    hwControl.gains.v_dc_in_ofs =  BUCK_CONFIG_V_DC_IN_OFFS;

    hwControl.gains.v_in_gain = BUCK_CONFIG_V_IN_GAIN;
    hwControl.gains.v_in_ofs =  BUCK_CONFIG_V_IN_OFFS;
}
//-----------------------------------------------------------------------------
//=============================================================================
