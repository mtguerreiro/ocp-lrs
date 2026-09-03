
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlBoostCascFblin.h"
#include "fsbuckboostConfig.h"

#include "fsbuckboost.h"
#include "controller/controller.h"

#include "utils/dfilt.h"

#include "string.h"
#include <stdio.h>
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

typedef struct{

    float n;

    // Voltage PI
    float kp_v;
    float ki_v;

    // Current loop gain
    float kc;

    // Converter parameters
    float L;

    // Limits
    float umin;
    float umax;

    // Sampling
    float Ts;

} ctlparams_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static uint32_t k;
static float il_ref = 0.0f;
static float ev_int = 0.0f;

static ctlparams_t params = {

    .n = 10.0f,

    .kp_v = 0.2f,
    .ki_v = 40.0f,

    .kc = 8000.0f,

    .L = 15e-6f,

    .umin = 0.05f,
    .umax = 0.95f,

    .Ts = 10e-6f
};
//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinInit(void){

    return 0;
}
//-----------------------------------------------------------------------------

int32_t fsbuckboostControlBoostCascFblinRun(
    void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m =
        (fsbuckboostConfigMeasurements_t *)meas;

    fsbuckboostConfigControl_t *o =
        (fsbuckboostConfigControl_t *)outputs;

    // Current tracking error
    float ei = il_ref - m->il;

    // Desired current dynamics
    float idot_des = params.kc * ei;

    // Prevent division by zero
    float vo = m->v_out;

    if(vo < 1.0f)
        vo = 1.0f;

    // Feedback linearization
    float u = 1.0f
        - (m->v_in - params.L * idot_des) / vo;

    // Saturation
    if(u > params.umax)
        u = params.umax;

    if(u < params.umin)
        u = params.umin;

    // PWM output
    o->u = u;

    // Trigger outer loop
    if( ((uint32_t)params.n) != 0 ){

        if( k == 0 )
            fsbuckboostTrigSwIrq();

        k++;

        if( k >= ((uint32_t)params.n) )
            k = 0;
    }

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostCascFblinRun2(
    void *meas, int32_t nmeas,
    void *refs, int32_t nrefs
){

    (void)nmeas;
    (void)nrefs;

    fsbuckboostConfigMeasurements_t *m =
        (fsbuckboostConfigMeasurements_t *)meas;

    fsbuckboostConfigReferences_t *r =
        (fsbuckboostConfigReferences_t *)refs;

    // Outer-loop sample time
    float Ts_outer = params.Ts * params.n;

    // Voltage error
    float ev = r->v_out - m->v_out;

    // Integrator
    ev_int += ev * Ts_outer;

    // PI controller
    il_ref =
          params.kp_v * ev
        + params.ki_v * ev_int;

    // Current-reference saturation
    if(il_ref > 20.0f)
        il_ref = 20.0f;

    if(il_ref < 0.0f)
        il_ref = 0.0f;
    
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinSetParams(void *buffer, uint32_t size){

    if( size != sizeof(ctlparams_t) ) return -1;
    memcpy( (void *)&params, buffer, sizeof(ctlparams_t) );

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinGetParams(void *buffer, uint32_t size){

    if( size < sizeof(ctlparams_t) ) return -1;

    memcpy(buffer, (void *)&params, sizeof(ctlparams_t));

    return sizeof(ctlparams_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostCascFblinReset(void){

    il_ref = 0.0f;
    ev_int = 0.0f;

    k = 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    k = 0;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostCascFblinGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlBoostCascFblinInit;
    cbs->run = fsbuckboostControlBoostCascFblinRun;
    cbs->run2 = fsbuckboostControlBoostCascFblinRun2;
    cbs->setParams = fsbuckboostControlBoostCascFblinSetParams;
    cbs->getParams = fsbuckboostControlBoostCascFblinGetParams;
    cbs->reset = fsbuckboostControlBoostCascFblinReset;
    cbs->firstEntry = fsbuckboostControlBoostCascFblinFirstEntry;
    cbs->lastExit = fsbuckboostControlBoostCascFblinLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
