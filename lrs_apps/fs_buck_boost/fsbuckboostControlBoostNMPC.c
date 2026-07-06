
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
    float dt;
    float rw;
    float L;
    float Co;
    float i_abs;
    float v_norm;
    float i_norm;
    float dt_obs;
    float k1;
    float k2;

}ctlparams_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static uint32_t k;
static uint32_t n;
static float duty;

static ctlparams_t params = {
    .n = 10.0f,
    .dt = 1.0f / 5e3f,
    .rw = 150.0f,
    .L = 100e-6f,
    .Co = 430e-6f,
    .i_abs = 3.5f,
    .v_norm = 24.0f,
    .i_norm = 4.0f,
    .dt_obs = 1.0f / 50e3f,
    .k1 = 0.1067f,
    .k2 = 49.0749f
};

TYPE_GRAMPC_POINTER(grampc);

uparams_t uparams = {
    .rw = 150.0f,
    .L = 100e-6f,
    .Co = 430e-6f,
    .i_abs = 3.5f,
    .v_norm = 24.0f,
    .i_norm = 4.0f,
    .io = 0.0f,
    .v_in = 0.0f,
    .d = 0.0f
};

typeUSERPARAM *userparam = &uparams;

static uint32_t isinit = 1;
static uint32_t first_enter = 1;

// static float k1 = 0.1067f;
// static float k2 = 49.0749f;
static float x1_hat = 0.0f;
static float d_hat = 0.0f;
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

    grampc_setopt_string(grampc, "Integrator", "erk1");
    // grampc_setopt_string(grampc, "IntegratorCost", "simpson");

    // grampc_setopt_string(grampc, "ConstraintsHandling", "extpen");

    /********* estimate and set PenaltyMin *********/
    grampc_estim_penmin(grampc, 1);

    // grampc_printopt(grampc);
    // grampc_printparam(grampc);

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

    float x1 = m->il;
    float x2 = m->v_dc_out;

    float x1_hat_1, d_hat_1;

    x1_hat_1 = x1_hat + params.dt_obs / params.L * ( -(1.0 - duty) * x2 + m->v_in + d_hat + params.k1 * (x1 - x1_hat) );
    d_hat_1 = d_hat + params.dt_obs * params.k2 * ( x1 - x1_hat);
    // printf("x1_hat: %.4f\td_hat: %.4f\n\r", x1_hat_1, d_hat_1);

    x1_hat = x1_hat_1;
    d_hat = d_hat_1;

    if( n != 0 ){
        if( k == 0 )
            fsbuckboostTrigSwIrq();

        k++;
        if( k >= n ){
            k = 0;
            o->u = duty;
        }
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

    float x1_1 = m->il;
    float x2_1 = m->v_dc_out;
    float x2_ref = r->v_out;

    float x1, x2;
    x1 = x1_1 + params.dt / params.L  * (-(1.0 - duty)*x2_1 + m->v_in + d_hat);
    x2 = x2_1 + params.dt / params.Co * ( (1.0 - duty)*x1_1 - m->io);

    uparams.v_in = m->v_in;
    uparams.io = m->io;
    uparams.d = d_hat;

    /* Initial conditions */
    x0[0] = x1 / params.i_norm;
    x0[1] = x2 / params.v_norm;
    u0[0] = duty;
    grampc_setparam_real_vector(grampc, "x0", x0);
    grampc_setparam_real_vector(grampc, "u0", u0);

    /* Set final state */
    float uss = 1.0f - (m->v_in + d_hat) / x2_ref;
    float iss = m->io / (1.0f - uss);
    xs[0] = iss / params.i_norm;
    xs[1] = x2_ref / params.v_norm;
    us[0] = uss;
    grampc_setparam_real_vector(grampc, "xdes", xs);
    grampc_setparam_real_vector(grampc, "udes", us);

    /* Bounds for u */
#ifdef EN_STATE_CNT
    umax[0] = 1.0f;
    umin[0] = 0.0f;
#else
    umax[0] = 1.0f - (1.0f/x2) * (d_hat + m->v_in - params.L * ( params.i_abs - x1) / params.dt);
    if( umax[0] > 1.0f ) umax[0] = 1.0f;
    umin[0] = 1.0f - (1.0f/x2) * (d_hat + m->v_in - params.L * (-params.i_abs - x1) / params.dt);
    if( umin[0] < 0.0f ) umin[0] = 0.0f;
    // float x1_1, x2_1;
    // x1_1 = x1 + params.dt / params.L * (-(1.0 - duty)*x2 + m->v_in + d_hat);
    // x2_1 = x2 + params.dt / params.Co * ( (1.0 - duty)*x1 - m->io);
    // umax[0] = 1.0f - (1.0f/x2_1) * (m->v_in - params.L * ( params.i_abs - x1_1) / params.dt + d_hat);
    // if( umax[0] > 1.0f ) umax[0] = 1.0f;
    // umin[0] = 1.0f - (1.0f/x2_1) * (m->v_in - params.L * (-params.i_abs - x1_1) / params.dt + d_hat);
    // if( umin[0] < 0.0f ) umin[0] = 0.0f;
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

    uparams.rw = params.rw;
    uparams.L = params.L;
    uparams.Co = params.Co;
    uparams.i_abs = params.i_abs;
    uparams.v_norm = params.v_norm;
    uparams.i_norm = params.i_norm;

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

    (void)nmeas;
    (void)refs;
    (void)nrefs;
    (void)outputs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    float x2 = m->v_dc_out;
    float x2_ref = r->v_out;

    n = (uint32_t)params.n;
    k = 0;

    duty = (x2 - m->v_in) / x2;

    x1_hat = m->il;
    d_hat = 0.0f;

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
