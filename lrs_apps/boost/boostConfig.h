/*
 * boostConfig.h
 *
 */

#ifndef BOOST_CONFIG_H_
#define BOOST_CONFIG_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    float i_o;
    float i_l;
    
    float v_dc_in;
    float v_in;

    float v_dc_out;
    float v_out;
}boostConfigMeasurements_t;

typedef struct{
    float u;
}boostConfigControl_t;

typedef struct{
    float v_o;
}boostConfigReferences_t;

typedef struct{

    float v_in_gain;
    float v_in_ofs;

    float v_dc_in_gain;
    float v_dc_in_ofs;

    float v_dc_out_gain;
    float v_dc_out_ofs;

    float v_out_gain;
    float v_out_ofs;
    
    float i_l_gain;
    float i_l_ofs;

    float i_l_avg_gain;
    float i_l_avg_ofs;

    float i_o_gain;
    float i_o_ofs;

    float unit;

}boostConfigMeasGains_t;

#define BOOST_CONFIG_V_IN_GAIN          0.008020949483123596f
#define BOOST_CONFIG_V_IN_OFFS          0.011598827786870344f

#define BOOST_CONFIG_V_DC_IN_GAIN       0.008028708880696712f
#define BOOST_CONFIG_V_DC_IN_OFFS       0.00787639255415567f

#define BOOST_CONFIG_V_DC_OUT_GAIN      0.012142125632366019f
#define BOOST_CONFIG_V_DC_OUT_OFFS      0.003158288102495688f

#define BOOST_CONFIG_V_OUT_GAIN         0.012070524741706293f
#define BOOST_CONFIG_V_OUT_OFFS         -0.0027503948329898127f

#define BOOST_CONFIG_IL_GAIN            -0.024929819910083854f
#define BOOST_CONFIG_IL_OFFS            50.853921803119505f

#define BOOST_CONFIG_IL_AVG_GAIN        1.0f
#define BOOST_CONFIG_IL_AVG_OFFS        0.0f

#define BOOST_CONFIG_IO_GAIN            0.011056216067557635f
#define BOOST_CONFIG_IO_OFFS            -22.730251335732746f

#define BOOST_CONFIG_L                 ((float)(47e-6))
#define BOOST_CONFIG_C                 ((float)(156.6e-6))

#define BOOST_CONFIG_I_LIM             (25.0f)
#define BOOST_CONFIG_V_LIM             (50.0f)

#define BOOST_CONFIG_SHUTDOWN_U_DEC     0.005
//=============================================================================

#endif /* BOOST_CONFIG_H_ */
