
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckControlPlecs.h"
#include "buckConfig.h"

#include "controller/controller.h"

#include "plecs/Plecs_controller.h"

//============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckControlPlecsInit(void){

    Plecs_controller_initialize(0);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlPlecsRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    buckConfigMeasurements_t *m = (buckConfigMeasurements_t *)meas;
    buckConfigControl_t *o = (buckConfigControl_t *)outputs;
    buckConfigReferences_t *r = (buckConfigReferences_t *)refs;

    Plecs_controller_U.hw_inputs[0] = m->il;
    Plecs_controller_U.hw_inputs[1] = 0;
    Plecs_controller_U.hw_inputs[2] = m->v_in;
    Plecs_controller_U.hw_inputs[3] = m->v_dc_in;
    Plecs_controller_U.hw_inputs[4] = m->v_dc_out;
    Plecs_controller_U.hw_inputs[5] = m->v_out;

    Plecs_controller_U.ref = r->v_out;

    Plecs_controller_step();

    o->u = Plecs_controller_Y.hw_outputs[0];

    return sizeof(buckConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t buckControlPlecsSetParams(void *buffer, uint32_t size){

    (void)buffer;
    (void)size;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlPlecsGetParams(void *buffer, uint32_t size){

    (void)buffer;
    (void)size;

    return 0;
}
//-----------------------------------------------------------------------------
void buckControlPlecsReset(void){

    Plecs_controller_initialize(0);
}
//-----------------------------------------------------------------------------
void buckControlPlecsGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = buckControlPlecsInit;
    cbs->run = buckControlPlecsRun;
    cbs->setParams = buckControlPlecsSetParams;
    cbs->getParams = buckControlPlecsGetParams;
    cbs->reset = buckControlPlecsReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
