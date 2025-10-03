/*
 * buckHwIf.c
 *
 */
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckHwIf.h"

#include "c2000/buckHw.h"

#include "buckConfig.h"

#include "rp/rp.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    rphandle_t handles[BUCK_HW_IF_END];
    rpctx_t rp;
}buckHwIf_t;

typedef struct{

    buckHwIf_t interface;
}buckHwIfControl_t;

static buckHwIfControl_t hwControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t buckHwIfSetPwmEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetPwmEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t buckHwIfInitialize(void){

    /* Initializes the request processor */
    rpInitialize(&hwControl.interface.rp, BUCK_HW_IF_END, hwControl.interface.handles);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_PWM_ENABLE, buckHwIfSetPwmEnable);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_PWM_ENABLE, buckHwIfGetPwmEnable);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_PWM_FREQ, buckHwIfSetPwmFrequency);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_PWM_FREQ, buckHwIfGetPwmFrequency);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_PWM_DUTY, buckHwIfSetPwmDutyCycle);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_PWM_DUTY, buckHwIfGetPwmDutyCycle);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_PWM_DEAD_TIME, buckHwIfSetPwmDeadTime);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_PWM_DEAD_TIME, buckHwIfGetPwmDeadTime);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_INPUT_RELAY, buckHwIfSetInputRelay);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_INPUT_RELAY, buckHwIfGetInputRelay);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_OUTPUT_RELAY, buckHwIfSetOutputRelay);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_OUTPUT_RELAY, buckHwIfGetOutputRelay);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_MEAS_GAINS, buckHwIfSetMeasGains);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_MEAS_GAINS, buckHwIfGetMeasGains);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_CLEAR_STATUS, buckHwIfClearStatus);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_STATUS, buckHwIfGetStatus);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

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
static int32_t buckHwIfSetPwmEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t enable;

    if( insize != sizeof(enable) ) return -1;

    memcpy( (void *)&enable, in, sizeof(enable) );

    buckHwSetPwmEnable(enable & 0x01);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetPwmEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t enable;

    enable = buckHwGetPwmEnable();

    if( maxoutsize < sizeof(enable) ) return -1;

    memcpy( *out, (void *)&enable, sizeof(enable) );

    return sizeof(enable);
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t freq;

    if( insize != sizeof(freq) ) return -1;

    memcpy( (void *)&freq, in, sizeof(freq) );

    buckHwSetPwmFrequency(freq);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t freq;

    freq = buckHwGetPwmFrequency();

    if( maxoutsize < sizeof(freq) ) return -1;

    memcpy( *out, (void *)&freq, sizeof(freq) );

    return sizeof(freq);
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    float duty;

    if( insize != sizeof(duty) ) return -1;

    memcpy( (void *)&duty, in, sizeof(duty) );

    buckHwSetPwmDuty(duty);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    float duty;

    duty = buckHwGetPwmDuty();

    if( maxoutsize < sizeof(duty) ) return -1;

    memcpy( *out, (void *)&duty, sizeof(duty) );

    return sizeof(duty);
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    float deadtime;

    if( insize != sizeof(deadtime) ) return -1;

    memcpy( (void *)&deadtime, in, sizeof(deadtime) );

    buckHwSetPwmDeadTime(deadtime);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    float deadtime;

    deadtime = buckHwGetPwmDeadTime();

    if( maxoutsize < sizeof(deadtime) ) return -1;

    memcpy( *out, (void *)&deadtime, sizeof(deadtime) );

    return sizeof(deadtime);
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t state;

    if( insize != sizeof(state) ) return -1;

    memcpy( (void *)&state, in, sizeof(state) );

    buckHwSetInputRelay(state);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t state;

    state = buckHwGetInputRelay();

    if( maxoutsize < sizeof(state) ) return -1;

    memcpy( *out, (void *)&state, sizeof(state) );

    return sizeof(state);
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    uint32_t state;

    if( insize != sizeof(state) ) return -1;

    memcpy( (void *)&state, in, sizeof(state) );

    buckHwSetOutputRelay(state);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    uint32_t state;

    state = buckHwGetOutputRelay();

    if( maxoutsize < sizeof(state) ) return -1;

    memcpy( *out, (void *)&state, sizeof(state) );

    return sizeof(state);
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)out;
    (void)maxoutsize;
    buckConfigMeasGains_t gains;

    if( insize != sizeof(gains) ) return -1;

    memcpy( (void* )&gains, in, sizeof(gains) );

    buckHwSetMeasGains(&gains);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    buckConfigMeasGains_t gains;

    buckHwGetMeasGains(&gains);

    if( maxoutsize < sizeof(gains) ) return -1;

    memcpy( *out, (void *)&gains, sizeof(gains) );

    return sizeof(gains);
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    (void)out;
    (void)maxoutsize;

    buckHwStatusClear();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    (void)in;
    (void)insize;
    (void)out;
    (void)maxoutsize;

    uint32_t status;

    status = buckHwStatus();

    if( maxoutsize < sizeof(status) ) return -1;

    memcpy( *out, (void *)&status, sizeof(status) );

    return sizeof(status);
}
//-----------------------------------------------------------------------------
//=============================================================================
