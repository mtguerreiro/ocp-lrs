
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlPlecs.h"
#include "fsbuckboostConfig.h"

#include "controller/controller.h"

#include "plecs/Buck_controller.h"

//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsInit(void){

    Buck_controller_initialize(0);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    Buck_controller_U.hw_inputs[0] = m->ii;
    Buck_controller_U.hw_inputs[1] = m->il;
    Buck_controller_U.hw_inputs[2] = m->io;
    Buck_controller_U.hw_inputs[3] = m->v_in;
    Buck_controller_U.hw_inputs[4] = m->v_dc_out;
    Buck_controller_U.hw_inputs[5] = m->v_out;

    Buck_controller_U.ref = r->v_out;

    Buck_controller_step();

    o->u = Buck_controller_Y.hw_outputs[0];

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsSetParams(void *buffer, uint32_t size){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsGetParams(void *buffer, uint32_t size){

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlPlecsReset(void){

    Buck_controller_initialize(0);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlPlecsGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlPlecsInit;
    cbs->run = fsbuckboostControlPlecsRun;
    cbs->setParams = fsbuckboostControlPlecsSetParams;
    cbs->getParams = fsbuckboostControlPlecsGetParams;
    cbs->reset = fsbuckboostControlPlecsReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
