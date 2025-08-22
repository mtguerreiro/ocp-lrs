
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlBoostEnergy.h"
#include "fsbuckboostConfig.h"

#include "controller/controller.h"

#include "utils/dfilt.h"

#include "string.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    float k1;
    float k2;
    float k3;
    float dt;

    float C;
    float L;

    float alpha;
    float filt_en;

    float kd;
}ctlparams_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float vo_r = 0.0f;
static float il_r = 0.0f;

static float yr = 0.0f;
static float y = 0.0f, y_dot = 0.0f;

static float u = 0.0f, v = 0.0f;

static float e = 0.0f, e_dot = 0.0f;

static float io_filt = 0.0f;

static uint32_t first_enter = 0;

static ctlparams_t params = {
    .k1 = 7743839.640425906f, .k2 = 5599.999998136721, .k3 = -5375358561.238816, .dt = 1.0f / 100e3,
    .C = 100e-6, .L = 15e-6, .alpha = 0.78f, .filt_en = 0.0f, .kd = 5.0f
};
//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyInit(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    if(first_enter == 0 ){
        io_filt = m->io;
    }

    if( params.filt_en > 0.5f ) io_filt = dfiltExpMovAvg(m->io, io_filt, params.alpha);
    else io_filt = m->io;

    /* References */
    il_r = m->v_dc_out * io_filt / m->v_in;

    vo_r = r->v_out - params.kd * io_filt;

    yr = (1. / 2.) * params.C * vo_r * vo_r + (1. / 2.) * params.L * il_r * il_r;

    /* States */
    y = (1. / 2.) * params.C * m->v_dc_out * m->v_dc_out + (1. / 2.) * params.L * m->il * m->il;
    y_dot = m->v_in * m->il - m->v_dc_out * io_filt;

    if( first_enter == 0 ){
        e =  -(params.k1 * y + params.k2 * y_dot) / params.k3;
        first_enter = 1;
    }

    e_dot = yr - y;
    e = e + params.dt * e_dot;

    /* Control law */
    v = - params.k1 * y - params.k2 * y_dot - params.k3 * e;

    /* Feedback linearization */
    u = 1.0f - 1.0f / m->v_dc_out * (m->v_in - params.L / m->v_in * v);

    if( u > 1.0f ) u = 1.0f;
    else if( u < 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergySetParams(void *buffer, uint32_t size){

    if( size != sizeof(ctlparams_t) ) return -1;
    memcpy( (void *)&params, buffer, sizeof(ctlparams_t) );

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyGetParams(void *buffer, uint32_t size){

    if( size < sizeof(ctlparams_t) ) return -1;

    memcpy(buffer, (void *)&params, sizeof(ctlparams_t));

    return sizeof(ctlparams_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostEnergyReset(void){

    e = 0.0f;

    first_enter = 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyFirstEntry(void *meas, int32_t nmeas,
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
int32_t fsbuckboostControlBoostEnergyLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    first_enter = 0;

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostEnergyGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlBoostEnergyInit;
    cbs->run = fsbuckboostControlBoostEnergyRun;
    cbs->setParams = fsbuckboostControlBoostEnergySetParams;
    cbs->getParams = fsbuckboostControlBoostEnergyGetParams;
    cbs->reset = fsbuckboostControlBoostEnergyReset;
    cbs->firstEntry = fsbuckboostControlBoostEnergyFirstEntry;
    cbs->lastExit = fsbuckboostControlBoostEnergyLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
