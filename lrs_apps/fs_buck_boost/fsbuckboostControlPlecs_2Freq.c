
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlPlecs_2Freq.h"
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
static uint32_t n;
static float internal_ref = 0;
static float d;
static ctlparams_t params = {

    .n = 10.0f,


};
//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControl2FreqPlecsInit(void){
    // printf("Init called\n");
    Plecs_controller_inner_loop_initialize(0);
    Plecs_controller_outer_loop_initialize(0);
    k = 0;
    n = (uint32_t)params.n;
    internal_ref = 0.0f;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsInnerLoopRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    Plecs_controller_inner_loop_U.hw_inputs[0] = m->ii;
    Plecs_controller_inner_loop_U.hw_inputs[1] = m->il;
    Plecs_controller_inner_loop_U.hw_inputs[2] = m->io;
    Plecs_controller_inner_loop_U.hw_inputs[3] = m->v_in;
    Plecs_controller_inner_loop_U.hw_inputs[4] = m->v_dc_out;
    Plecs_controller_inner_loop_U.hw_inputs[5] = m->v_out;


    Plecs_controller_inner_loop_U.internal_ref = internal_ref;
    // Plecs_controller_inner_loop_U.ref[0] = internal_ref;

    Plecs_controller_inner_loop_step();
    
     o->u = Plecs_controller_inner_loop_Y.hw_outputs[0];

    // Trigger slow loop
    if( n != 0 ){

        if( k == 0 )
            fsbuckboostTrigSwIrq();

        k++;

        // if(k == (n-1))
        //     o->u = Plecs_controller_inner_loop_Y.hw_outputs[0];

        if( k >= n )
            k = 0;
    }
   



    return sizeof(fsbuckboostConfigControl_t);
}
void fsbuckboostControlPlecsOuterLoopRun(
    void *meas,
    int32_t nmeas,
    void *refs,
    int32_t nrefs)
{
    (void)nmeas;
    (void)nrefs;

    fsbuckboostConfigMeasurements_t *m =
        (fsbuckboostConfigMeasurements_t *)meas;

    fsbuckboostConfigReferences_t *r =
        (fsbuckboostConfigReferences_t *)refs;

    Plecs_controller_outer_loop_U.hw_inputs[0] = m->ii;
    Plecs_controller_outer_loop_U.hw_inputs[1] = m->il;
    Plecs_controller_outer_loop_U.hw_inputs[2] = m->io;
    Plecs_controller_outer_loop_U.hw_inputs[3] = m->v_in;
    Plecs_controller_outer_loop_U.hw_inputs[4] = m->v_dc_out;
    Plecs_controller_outer_loop_U.hw_inputs[5] = m->v_out;


    Plecs_controller_outer_loop_U.ref = r->v_out;



    Plecs_controller_outer_loop_step();

    internal_ref = Plecs_controller_outer_loop_Y.internal_ref;

    // return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControl2FreqPlecsFirstEntry(
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

    n = (uint32_t)params.n;
    k = 0;
    internal_ref = 0.0f;

    // printf("PLECS CASCADE CONTROLLER 2Freq ENABLED\n");

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControl2FreqPlecsSetParams(void *buffer, uint32_t size)
{
    printf("SetParams");
    (void)buffer;
    (void)size;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControl2FreqPlecsGetParams(void *buffer, uint32_t size){
    printf("getParams");
   if(size < sizeof(ctlparams_t))
        return -1;

    memcpy(buffer, &params, sizeof(params));

    return sizeof(params);
}
//-----------------------------------------------------------------------------
void fsbuckboostControl2FreqPlecsReset(void){
    printf("reset");

    Plecs_controller_inner_loop_initialize(0);
    Plecs_controller_outer_loop_initialize(0);

    k = 0;
    internal_ref = 0.0f;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoost2FREQGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControl2FreqPlecsInit;
    cbs->run = fsbuckboostControlPlecsInnerLoopRun;
    cbs->run2 = fsbuckboostControlPlecsOuterLoopRun;
    cbs->setParams = fsbuckboostControl2FreqPlecsSetParams;
    cbs->getParams = fsbuckboostControl2FreqPlecsGetParams;
    cbs->reset = fsbuckboostControl2FreqPlecsReset;
    // cbs->firstEntry = 0;
    cbs->firstEntry = fsbuckboostControl2FreqPlecsFirstEntry;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
