
#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostControlEnergyInt.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include "boostConfig.h"

#include "dfilt.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

static float C = 150e-6;
static float L = 47e-6;

static float vo_r = 0.0f;
static float il_r = 0.0f;

static float yr = 0.0f;
static float y = 0.0f, y_dot = 0.0f;

static float u = 0.0f, v = 0.0f;

static float k1 = 7743839.640425906f, k2 = 5599.999998136721, k3 = -5375358561.238816;

static float dt = 1.0 / 100e3;

static float e = 0.0f, e_dot = 0.0f;

static float io_filt = 0.0f;

static float alpha = 0.78;
static uint32_t filt_en = 0;

static uint32_t first_enter = 0;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostControlEnergyIntInitialize(void){

}
//-----------------------------------------------------------------------------
int32_t boostControlEnergyIntSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    k1 = *p++;
    k2 = *p++;
    k3 = *p++;
    dt = *p++;

    C = *p++;
    L = *p++;

    alpha = *p++;
    filt_en = (uint32_t)*p++;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t boostControlEnergyIntGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

    float *p = (float *)out;

    *p++ = k1;
    *p++ = k2;
    *p++ = k3;
    *p++ = dt;

    *p++ = C;
    *p++ = L;

    *p++ = alpha;
    *p++ = (float)filt_en;

    return 32;
}
//-----------------------------------------------------------------------------
int32_t boostControlEnergyIntRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    boostConfigMeasurements_t *m = (boostConfigMeasurements_t *)meas;
    boostConfigReferences_t *r = (boostConfigReferences_t *)refs;
    boostConfigControl_t *o = (boostConfigControl_t *)outputs;

    if(first_enter == 0 ){
        io_filt = m->i_o;
    }

    if( filt_en != 0 ) io_filt = dfiltExpMovAvg(m->i_o, io_filt, alpha);
    else io_filt = m->i_o;

    /* References */
    il_r = m->v_out * io_filt / m->v_in;

    vo_r = r->v_o;

    yr = (1. / 2.) * C * vo_r * vo_r + (1. / 2.) * L * il_r * il_r;

    /* States */
    y = (1. / 2.) * C * m->v_dc_out * m->v_dc_out + (1. / 2.) * L * m->i_l * m->i_l;
    y_dot = m->v_dc_in * m->i_l - m->v_dc_out * io_filt;

    if( first_enter == 0 ){
        e =  -(k1 * y + k2 * y_dot) / k3;
        first_enter = 1;
    }

    e_dot = yr - y;
    e = e + dt * e_dot;

    /* Control law */
    v = - k1 * y - k2 * y_dot - k3 * e;

    /* Feedback linearization */
    u = 1 - 1 / m->v_dc_out * (m->v_dc_in - L / m->v_dc_in * v);

    if( u > 1.0f ) u = 1.0f;
    else if ( u < 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(boostConfigControl_t);
}
//-----------------------------------------------------------------------------
void boostControlEnergyIntReset(void){

    e = 0.0f;

    first_enter = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
