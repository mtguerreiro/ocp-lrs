
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
    float filt_coef;
    float filt_en;



} ctlparams_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static uint32_t k;
static uint32_t n;
static float z_int = 0.0f;
static float ki = 800.0f;

static float u_mpc;

static float ii_filt;

static ctlparams_t params = {

    .n = 10.0f,
    .filt_coef = 0.7f,
    .filt_en = 0,

};

static uint32_t isinit = 1;
static uint32_t first_enter = 1;

TYPE_GRAMPC_POINTER(grampc);

typeTime tic, toc;

/* Time variables */
ctypeRNum Thor = (typeRNum) 0.5e-3;  /* Prediction horizon */

ctypeRNum dt = (typeRNum)10e-6;  /* Sampling time */
typeRNum t = (typeRNum)0.0;         /* time at the current sampling step */

/********* Option definition *********/
/* Basic algorithmic options */
ctypeInt Nhor = (typeInt)6;        /* Number of steps for the system integration */
ctypeInt MaxGradIter = (typeInt)3;  /* Maximum number of gradient iterations */
ctypeInt MaxMultIter = (typeInt)1;  /* Maximum number of augmented Lagrangian iterations */

/* Constraints tolerances */
ctypeRNum ConstraintsAbsTol[2] = { 1e-4, 1e-4 };

/********* userparam *********/
uparams_t uparams = {
  .rw = 20.0f,
  .io = 0.0f,
  .v_in = 0.0f,
  .qz = 0.0f,
};

typeUSERPARAM *userparam = &uparams;
//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
static float solve_quadratic_u(float a, float b, float c)
{
    float disc;
    float u1, u2;

    /* If equation is nearly linear */
    if(fabs(a) < 1e-8f)
    {
        if(fabs(b) < 1e-8f)
            return 0.0f;

        return -c / b;
    }

    disc = b*b - 4.0f*a*c;

    /* Numerical protection */
    if(disc < 0.0f)
        disc = 0.0f;

    disc = sqrtf(disc);

    u1 = (-b + disc) / (2.0f * a);
    u2 = (-b - disc) / (2.0f * a);

    /* Select valid root */
    if(u1 >= 0.0f && u1 <= 1.0f)
        return u1;

    if(u2 >= 0.0f && u2 <= 1.0f)
        return u2;

    /* Fallback */
    if(u1 < 0.0f) return 0.0f;
    if(u1 > 1.0f) return 1.0f;

    return u1;
}
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
    

    // Trigger slow loop
    if( n != 0 ){

        if( k == 0 )
            fsbuckboostTrigSwIrq();

        k++;

        if(k == (n-1))
            o->u = u_mpc;

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
        
    typeRNum x0[3], xs[3];
    typeRNum u0[1], us[1];
    float umax[1], umin[1];

    if( params.filt_en != 0 ) ii_filt = dfiltExpMovAvg(m->ii, ii_filt, params.filt_coef);
    else ii_filt = m->ii;

    float x1 = ii_filt;
    float x2 = m->v_out - RCO*(m->ii - m->io);
    float x2_ref = r->v_out;

    uparams.io   = m->io;
    uparams.v_in = m->v_in;

    float Ts = TS ;

    float u_applied = u_mpc;
       
    /*mid-point Euler*/
    // float il_pred, vo_pred;

    

    // il_pred = x1 + Ts * (-(1.0f - u_applied) * x2 + m->v_in)/ L_inductor;
    // float il_avg = 0.5f * (x1 + il_pred);

    // vo_pred = x2 + Ts * ((1.0f - u_applied) * x1 - m->io)/ Co;
    // float vo_avg = 0.5f * (x2 + vo_pred);


    // /* RK2 prediction implementation */
    // float k1_i, k1_v;
    // k1_i =(-(1.0f - u_applied) * x2 + m->v_in)/ L_inductor;
    // k1_v = ((1.0f - u_applied) * x1 - m->io)/ Co;

    // float i_mid, v_mid;
    // i_mid = x1 + 0.5f * Ts * k1_i;
    // v_mid = x2 + 0.5f * Ts * k1_v;

    // float k2_i, k2_v;
    // k2_i = (-(1.0f - u_applied) * v_mid + m->v_in) / L_inductor;
    // k2_v = ((1.0f - u_applied) * i_mid - m->io)/ Co;

    // float il_rk2, vo_rk2;
    // il_rk2 = x1 + Ts * k2_i;
    // vo_rk2 = x2 + Ts * k2_v;
    // /* end of RK2 */
        float alpha = 1.0f - u_applied;

/* =========================================================
   Step 1: output voltage with capacitor ESR
   v_o = v_C + Rco * i_C
   i_C = alpha*i_L - i_o
   ========================================================= */
    float vo_k1 =
    x2
    + RCO * (alpha * x1 - m->io);

/* =========================================================
   k1
   ========================================================= */
    float k1_i =
    (
        m->v_in
        - RL * x1
        - alpha * vo_k1
    ) / L_inductor;

    float k1_v =
    (
        alpha * x1
        - m->io
    ) / Co;

    /* =========================================================
    Midpoint states
    ========================================================= */
    float i_mid = x1 + 0.5f * TS * k1_i;
    float v_mid = x2 + 0.5f * TS * k1_v;

    /* =========================================================
    Midpoint output voltage with ESR
    ========================================================= */
    float vo_k2 =
        v_mid
        + RCO * (alpha * i_mid - m->io);

    /* =========================================================
    k2
    ========================================================= */
    float k2_i =
    (
        m->v_in
        - RL * i_mid
        - alpha * vo_k2
    ) / L_inductor;

    float k2_v =
    (
        alpha * i_mid
        - m->io
    ) / Co;

    /* =========================================================
    RK2 update
    ========================================================= */
    float il_rk2 = x1 + TS * k2_i;
    float vo_rk2 = x2 + TS * k2_v;


    z_int += TS *ki* (x2_ref - vo_rk2); // update the integrator 
    // anti wind-up
    if(z_int > 5.0f) z_int = 5.0f;
    if(z_int < -5.0f) z_int = -5.0f;

    /* Initial conditions */
    x0[0] = il_rk2 / I_NORM;
    x0[1] = vo_rk2 / V_NORM;
    // x0[2] = 0.0f;
    u0[0] = u_mpc;
    grampc_setparam_real_vector(grampc, "x0", x0);
    grampc_setparam_real_vector(grampc, "u0", u0);

    /* Set final state */
    xs[0] = (m->io * x2_ref / m->v_in) / I_NORM;
    xs[1] = (x2_ref+ z_int) / V_NORM;
    // xs[2] = 0.0f;
    us[0] = (x2_ref - m->v_in) / (x2_ref);
    grampc_setparam_real_vector(grampc, "xdes", xs);
    grampc_setparam_real_vector(grampc, "udes", us);

    // // /* Bounds for u */
    // float a, b, c;
    // float A;
    // float iL_internalLimit = I_ABS + 0.0f ;

    // /* ---------- Upper bound ---------- */

    // A = (L_inductor / Ts) * (iL_internalLimit - il_rk2);

    // a =
    //     -(Ts * il_rk2) / (2.0f * Co);

    // b =
    // vo_rk2
    //     + (Ts * il_rk2) / Co
    //     - (Ts * m->io) / (2.0f * Co);

    // c =
    //     m->v_in
    //     - vo_rk2
    //     - (Ts * il_rk2) / (2.0f * Co)
    //     + (Ts * m->io) / (2.0f * Co)
    //     - A;

    // umax[0] = solve_quadratic_u(a, b, c);

    // if(umax[0] > 1.0f)
    //     umax[0] = 1.0f;

    // if(umax[0] < 0.0f)
    //     umax[0] = 0.0f;

    // A = (L_inductor / Ts) * (-iL_internalLimit - il_rk2);

    // c =
    //     m->v_in
    //     - vo_rk2
    //     - (Ts * il_rk2) / (2.0f * Co)
    //     + (Ts * m->io) / (2.0f * Co)
    //     - A;

    // umin[0] = solve_quadratic_u(a, b, c);

    // if(umin[0] > 1.0f)
    //     umin[0] = 1.0f;

    // if(umin[0] < 0.0f)
    //     umin[0] = 0.0f;


    /*bound for the control input*/
    // umax[0] = 1.0f - (1.0f/x2) * (m->v_in - L_inductor * ( I_ABS - x1) / TS);
    // if( umax[0] > 1.0f ) umax[0] = 1.0f;
    // umin[0] = 1.0f - (1.0f/x2) * (m->v_in - L_inductor * (-I_ABS - x1) / TS);
    // if( umin[0] < 0.0f ) umin[0] = 0.0f;
    /* ---------- Upper bound ---------- */
    umax[0] =
    1.0f
    - (1.0f / vo_rk2) *
    (
        m->v_in
        - RL * il_rk2
        - L_inductor * (I_ABS - il_rk2) / TS
    );

    if (umax[0] > 1.0f) umax[0] = 1.0f;
    if (umax[0] < 0.0f) umax[0] = 0.0f;

    /* ---------- Lower bound ---------- */
    umin[0] =
    1.0f
    - (1.0f / vo_rk2) *
    (
        m->v_in
        - RL * il_rk2
        - L_inductor * (-I_ABS - il_rk2) / TS
    );

    if (umin[0] > 1.0f) umin[0] = 1.0f;
    if (umin[0] < 0.0f) umin[0] = 0.0f;

    grampc_setparam_real_vector(grampc, "umax", umax);
    grampc_setparam_real_vector(grampc, "umin", umin);

    /* Solve */
    grampc_run(grampc);

     /* Apply control */
    u_mpc = grampc->sol->unext[0];
    // printf("xdes=%f x2=%f z=%f vo= %f\n",
    //    x2_ref,
    //    x2,
    //    z_int,
    //    m->v_out);
    //    printf("x0[2]=%f\n", x0[2]);

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

    // k = 0;
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
    // printf("NMPC ENABLED");


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
