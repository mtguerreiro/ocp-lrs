
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlBoostNMPC.h"
#include "fsbuckboostConfig.h"

#include "fsbuckboost.h"
#include "controller/controller.h"

#include "utils/dfilt.h"

#include "string.h"
#include <stdio.h>
#include <math.h>

#include "grampc.h"
#include "timing.h"

#ifdef TARGET_ZYNQ
#include "xiltimer.h"
#endif
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

static float u_mpc;

static ctlparams_t params = {

    .n = 20.0f,

};

static uint32_t isinit = 1;
static uint32_t first_enter = 1;

TYPE_GRAMPC_POINTER(grampc);

typeTime tic, toc;

/* Time variables */
ctypeRNum Thor = (typeRNum) 1e-3;  /* Prediction horizon */

ctypeRNum dt = (typeRNum)10e-6;  /* Sampling time */
typeRNum t = (typeRNum)0.0;         /* time at the current sampling step */

/********* Option definition *********/
/* Basic algorithmic options */
ctypeInt Nhor = (typeInt)21;        /* Number of steps for the system integration */
ctypeInt MaxGradIter = (typeInt)2;  /* Maximum number of gradient iterations */
ctypeInt MaxMultIter = (typeInt)1;  /* Maximum number of augmented Lagrangian iterations */

/* Constraints tolerances */
ctypeRNum ConstraintsAbsTol[2] = { 1e-4, 1e-4 };

/********* userparam *********/
uparams_t uparams = {
  .rw = 0.5f,
  .io = 0.0f,
  .v_in = 0.0f,
};

typeUSERPARAM *userparam = &uparams;
//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCInit(void){

    first_enter = 1;

    if( isinit == 0 ) return 0;
    isinit = 0;

    /********* grampc init *********/
    grampc_init(&grampc, userparam);


    /********* set parameters *********/
    grampc_setparam_real(grampc, "Thor", Thor);

    grampc_setparam_real(grampc, "dt", dt);
    grampc_setparam_real(grampc, "t0", t);

    /********* Option definition *********/
    grampc_setopt_int(grampc, "Nhor", Nhor);
    grampc_setopt_int(grampc, "MaxGradIter", MaxGradIter);
    grampc_setopt_int(grampc, "MaxMultIter", MaxMultIter);

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

int32_t fsbuckboostControlBoostNMPCRun(
    void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    (void)nmeas;
    (void)nrefs;
    (void)nmaxoutputs;

    fsbuckboostConfigMeasurements_t *m =
        (fsbuckboostConfigMeasurements_t *)meas;

    fsbuckboostConfigControl_t *o =
        (fsbuckboostConfigControl_t *)outputs;
    
    #ifdef TARGET_ZYNQ
        XTime t1, t2;
        XTime_GetTime(&t1);
    #else
        timer_now(&tic);
    #endif
   


    // PWM output
    o->u = u_mpc;

    // Trigger slow loop
    if( n != 0 ){

        if( k == 0 )
            fsbuckboostTrigSwIrq();

        k++;

        if( k >= n )
            k = 0;
    }
   

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostNMPCRun2(
    void *meas, int32_t nmeas,
    void *refs, int32_t nrefs
){

    (void)nmeas;
    (void)nrefs;

    fsbuckboostConfigMeasurements_t *m =
        (fsbuckboostConfigMeasurements_t *)meas;

    fsbuckboostConfigReferences_t *r =
        (fsbuckboostConfigReferences_t *)refs;


    // float Ts_MPC = params.Ts * params.n;
        
    typeRNum x0[2], xs[2];
    typeRNum u0[1], us[1];
    float umax[1], umin[1];

    float x1 = m->il;
    float x2 = m->v_out;
    float x2_ref = r->v_out;

    uparams.io   = m->io;
    uparams.v_in = m->v_in;

    float u_applied = u_mpc;


    /* Initial conditions */
    x0[0] = x1 / I_NORM;
    x0[1] = x2 / V_NORM;
    u0[0] = u_mpc;
    grampc_setparam_real_vector(grampc, "x0", x0);
    grampc_setparam_real_vector(grampc, "u0", u0);

    /* Set final state */
    xs[0] = (m->io * x2_ref / m->v_in) / I_NORM;
    xs[1] = x2_ref / V_NORM;
    us[0] = (x2_ref - m->v_in) / (x2_ref);
    grampc_setparam_real_vector(grampc, "xdes", xs);
    grampc_setparam_real_vector(grampc, "udes", us);


    /*Euler approach*/
    umax[0] = 1.0f - (1.0f/x2) * (m->v_in - L_inductor * ( I_ABS - x1) / TS);
    if( umax[0] > 1.0f ) umax[0] = 1.0f;
    umin[0] = 1.0f - (1.0f/x2) * (m->v_in - L_inductor * (-I_ABS - x1) / TS);
    if( umin[0] < 0.0f ) umin[0] = 0.0f;

    grampc_setparam_real_vector(grampc, "umax", umax);
    grampc_setparam_real_vector(grampc, "umin", umin);

    /* Solve */
    grampc_run(grampc);

     /* Apply control */
    u_mpc = grampc->sol->unext[0];

// #ifdef TARGET_ZYNQ
//     XTime_GetTime(&t2);
//     controllerdata->t_exec = ( (float) (t2 - t1) ) / ( (float) COUNTS_PER_SECOND ) / 1e-6;
// // #else
//     // timer_now(&toc);
//     // controllerdata->t_exec = timer_diff_ms(&tic, &toc) / 1e-3;
// #endif
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

    // il_ref = 0.0f;
    // ev_int = 0.0f;

    k = 0;
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
    printf("NMPC ENABLED");


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
