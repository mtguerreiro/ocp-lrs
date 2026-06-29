
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlBoostNMPC.h"
#include "fsbuckboostConfig.h"

#include "fsbuckboost.h"
#include "controller/controller.h"

#include "utils/dfilt.h"

#include "string.h"

#include "grampc.h"
#include "controller_params.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    float n;
}ctlparams_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static uint32_t k;
static uint32_t n;

static float duty;

static ctlparams_t params = {
    .n = 10.0f
};

TYPE_GRAMPC_POINTER(grampc);

uparams_t uparams = {
    .rw = RW,
    .io = 0.0f,
    .v_in = 0.0f,
};

typeUSERPARAM *userparam = &uparams;

static uint32_t isinit = 1;
static uint32_t first_enter = 1;

//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCInit(void){

    ctypeRNum ConstraintsAbsTol[2] = { 1e-2, 1e-2 };

    first_enter = 1;

    if( isinit == 0 ) return 0;
    isinit = 0;

    /********* grampc init *********/
    grampc_init(&grampc, userparam);


    /********* set parameters *********/
    grampc_setparam_real(grampc, "Thor", T_HORIZON);

    grampc_setparam_real(grampc, "dt", DT_GRAMPC);
    grampc_setparam_real(grampc, "t0", 0.0f);

    /* Sets initial values for penmin estimation */
    float umax[1] = {1.0f}, umin[1] = {0.0f};
    float u0[1] = {0.8f}; float udes[1] = {0.8f};

    grampc_setparam_real_vector(grampc, "u0", u0);
    grampc_setparam_real_vector(grampc, "udes", udes);
    grampc_setparam_real_vector(grampc, "umax", umax);
    grampc_setparam_real_vector(grampc, "umin", umin);

    float x0[NX] = {0.9f, 0.9f};
    float xdes[NX] = {1.0f, 1.0f};

    grampc_setparam_real_vector(grampc, "x0", x0);
    grampc_setparam_real_vector(grampc, "xdes", xdes);

    /********* Option definition *********/
    grampc_setopt_int(grampc, "Nhor", N_HORIZON);
    grampc_setopt_int(grampc, "MaxGradIter", MAX_GRAD_ITER);
    grampc_setopt_int(grampc, "MaxMultIter", MAX_MULT_ITER);

    grampc_setopt_string(grampc, "TerminalCost", "off");
    grampc_setopt_string(grampc, "TerminalEqualityConstraints", "off");
    grampc_setopt_string(grampc, "TerminalInequalityConstraints", "off");

    grampc_setopt_real_vector(grampc, "ConstraintsAbsTol", ConstraintsAbsTol);

    grampc_setopt_string(grampc, "Integrator", "erk2");
    // grampc_setopt_string(grampc, "IntegratorCost", "simpson");

    // grampc_setopt_string(grampc, "ConstraintsHandling", "extpen");

    /********* estimate and set PenaltyMin *********/
    grampc_estim_penmin(grampc, 1);

    grampc_printopt(grampc);
    grampc_printparam(grampc);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    if( n != 0 ){
        if( k == 0 )
            fsbuckboostTrigSwIrq();

        k++;
        if( k == (n - 1) ) o->u = duty;
        if( k >= n ) k = 0;
    }

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostNMPCRun2(
    void *meas, int32_t nmeas,
    void *refs, int32_t nrefs
){

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    typeRNum x0[2], xs[2];
    typeRNum u0[1], us[1];
    float umax[1], umin[1];

    float x1 = m->il;
    float x2 = m->v_dc_out;
    float x2_ref = r->v_out;

    // float x1, x2;
    // x1 = x1_1 + L  / TS * (-(1.0 - duty)*x2_1 + m->v_in);
    // x2 = x2_1 + Co / TS * ( (1.0 - duty)*x1_1 - m->io);

    if(first_enter == 1 ){
        first_enter = 0;
        duty = (x2 - m->v_in) / x2;
    }

    uparams.v_in = m->v_in;
    uparams.io = m->io;

    /* Initial conditions */
    x0[0] = x1 / I_NORM;
    x0[1] = x2 / V_NORM;
    u0[0] = duty;
    grampc_setparam_real_vector(grampc, "x0", x0);
    grampc_setparam_real_vector(grampc, "u0", u0);

    /* Set final state */
    xs[0] = (m->io * x2_ref / m->v_in) / I_NORM;
    xs[1] = x2_ref / V_NORM;
    us[0] = (x2_ref - m->v_in) / (x2_ref);
    grampc_setparam_real_vector(grampc, "xdes", xs);
    grampc_setparam_real_vector(grampc, "udes", us);

    /* Bounds for u */
#ifdef EN_STATE_CNT
    umax[0] = 1.0f;
    umin[0] = 0.0f;
#else
    // umax[0] = 1.0f - (1.0f/x2) * (m->v_in - L * ( I_ABS - x1) / TS);
    // if( umax[0] > 1.0f ) umax[0] = 1.0f;
    // umin[0] = 1.0f - (1.0f/x2) * (m->v_in - L * (-I_ABS - x1) / TS);
    // if( umin[0] < 0.0f ) umin[0] = 0.0f;
    float x1_1, x2_1;
    x1_1 = x1 + L  / TS * (-(1.0 - duty)*x2 + m->v_in - 25e-3*x1);
    x2_1 = x2 + Co / TS * ( (1.0 - duty)*x1 - m->io);
    umax[0] = 1.0f - (1.0f/x2_1) * (m->v_in - L * ( I_ABS - x1_1) / TS - 25e-3*x1_1);
    if( umax[0] > 1.0f ) umax[0] = 1.0f;
    umin[0] = 1.0f - (1.0f/x2_1) * (m->v_in - L * (-I_ABS - x1_1) / TS - 25e-3*x1_1);
    if( umin[0] < 0.0f ) umin[0] = 0.0f;
#endif

    grampc_setparam_real_vector(grampc, "umax", umax);
    grampc_setparam_real_vector(grampc, "umin", umin);

    /* Solve */
    grampc_run(grampc);

    /* Apply control */
    duty = grampc->sol->unext[0];

}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCSetParams(void *buffer, uint32_t size){

    if( size != sizeof(ctlparams_t) ) return -1;
    memcpy( (void *)&params, buffer, sizeof(ctlparams_t) );

    n = (uint32_t) params.n;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCGetParams(void *buffer, uint32_t size){

    if( size < sizeof(ctlparams_t) ) return -1;

    memcpy(buffer, (void *)&params, sizeof(ctlparams_t));

    return sizeof(ctlparams_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostNMPCReset(void){

}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)meas;
    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    n = (uint32_t)params.n;
    k = 0;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCLastExit(void *meas, int32_t nmeas,
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
void fsbuckboostControlBoostNMPCGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlBoostNMPCInit;
    cbs->run = fsbuckboostControlBoostNMPCRun;
    cbs->run2 = fsbuckboostControlBoostNMPCRun2;
    cbs->setParams = fsbuckboostControlBoostNMPCSetParams;
    cbs->getParams = fsbuckboostControlBoostNMPCGetParams;
    cbs->reset = fsbuckboostControlBoostNMPCReset;
    cbs->firstEntry = fsbuckboostControlBoostNMPCFirstEntry;
    cbs->lastExit = fsbuckboostControlBoostNMPCLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
