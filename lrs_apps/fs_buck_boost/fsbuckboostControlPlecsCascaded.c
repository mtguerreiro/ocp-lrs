
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlPlecsCascaded.h"
#include "fsbuckboostConfig.h"

#include "fsbuckboost.h"
#include "controller/controller.h"

#include "utils/dfilt.h"

#include "string.h"
#include <stdio.h>
#include <math.h>

#include "plecs/Plecs_controller_inner_loop.h"
#include "plecs/Plecs_controller_outer_loop.h"

//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    float n;

} ctlparams_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static uint32_t k;
static float internal_ref = 0;
static ctlparams_t params;
//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsCascadedInit(void){

    Plecs_controller_inner_loop_initialize(0);
    Plecs_controller_outer_loop_initialize(0);

    k = 0;
    internal_ref = 0.0f;
    params.n = roundf(Plecs_controller_outer_loop_sampleTime / Plecs_controller_inner_loop_sampleTime);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsCascadedInnerLoopRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;

    Plecs_controller_inner_loop_U.hw_inputs[0] = m->ii;
    Plecs_controller_inner_loop_U.hw_inputs[1] = m->il;
    Plecs_controller_inner_loop_U.hw_inputs[2] = m->io;
    Plecs_controller_inner_loop_U.hw_inputs[3] = m->v_in;
    Plecs_controller_inner_loop_U.hw_inputs[4] = m->v_dc_out;
    Plecs_controller_inner_loop_U.hw_inputs[5] = m->v_out;

    Plecs_controller_inner_loop_U.internal_ref = internal_ref;

    Plecs_controller_inner_loop_step();

    o->u = Plecs_controller_inner_loop_Y.hw_outputs[0];

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
void fsbuckboostControlPlecsCascadedOuterLoopRun(void *meas, int32_t nmeas,
    void *refs,
    int32_t nrefs){

    (void)nmeas;
    (void)nrefs;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    Plecs_controller_outer_loop_U.hw_inputs[0] = m->ii;
    Plecs_controller_outer_loop_U.hw_inputs[1] = m->il;
    Plecs_controller_outer_loop_U.hw_inputs[2] = m->io;
    Plecs_controller_outer_loop_U.hw_inputs[3] = m->v_in;
    Plecs_controller_outer_loop_U.hw_inputs[4] = m->v_dc_out;
    Plecs_controller_outer_loop_U.hw_inputs[5] = m->v_out;

    Plecs_controller_outer_loop_U.ref = r->v_out;

    Plecs_controller_outer_loop_step();

    internal_ref = Plecs_controller_outer_loop_Y.internal_ref;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsCascadedFirstEntry(
    void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs)
{
    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    k = 0;
    internal_ref = 0.0f;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsCascadedSetParams(void *buffer, uint32_t size)
{
    (void)buffer;
    (void)size;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsCascadedGetParams(void *buffer, uint32_t size){

    if(size < sizeof(ctlparams_t))
        return -1;

    memcpy(buffer, &params, sizeof(params));

    return sizeof(params);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlPlecsCascadedReset(void){

    Plecs_controller_inner_loop_initialize(0);
    Plecs_controller_outer_loop_initialize(0);

    k = 0;
    internal_ref = 0.0f;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlPlecsCascadedGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlPlecsCascadedInit;
    cbs->run = fsbuckboostControlPlecsCascadedInnerLoopRun;
    cbs->run2 = fsbuckboostControlPlecsCascadedOuterLoopRun;
    cbs->setParams = fsbuckboostControlPlecsCascadedSetParams;
    cbs->getParams = fsbuckboostControlPlecsCascadedGetParams;
    cbs->reset = fsbuckboostControlPlecsCascadedReset;
    cbs->firstEntry = fsbuckboostControlPlecsCascadedFirstEntry;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
