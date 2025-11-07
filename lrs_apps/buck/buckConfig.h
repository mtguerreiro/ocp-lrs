/*
 * buckConfig.h
 *
 */

#ifndef BUCK_CONFIG_H_
#define BUCK_CONFIG_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"

#include "ocpConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    
    float io;
    float il;
    
    float v_dc_out;
    float v_out;

    float v_dc_in;
    float v_in;
    
}buckConfigMeasurements_t;

typedef struct{
    float u;
}buckConfigControl_t;

typedef struct{
    float v_out;
}buckConfigReferences_t;

typedef struct{

    float io_gain;
    float io_ofs;
    
    float il_gain;
    float il_ofs;

    float v_dc_out_gain;
    float v_dc_out_ofs;

    float v_out_gain;
    float v_out_ofs;

    float v_dc_in_gain;
    float v_dc_in_ofs;

    float v_in_gain;
    float v_in_ofs;
    
    float il_avg_gain;
    float il_avg_ofs;

}buckConfigMeasGains_t;

#define BUCK_CONFIG_IO_AVG_GAIN       (1.0f)
#define BUCK_CONFIG_IO_AVG_OFFS       (0.0f)

#define BUCK_CONFIG_IL_GAIN           (3.0f * (5.9f / 3.9f) / 4095.0f / 50e-3)
#define BUCK_CONFIG_IL_OFFS           (-(2.49f / 50e-3 + 1.4165f))

#define BUCK_CONFIG_V_DC_OUT_GAIN     (3.0f * (5.83f / 0.6f) / 4095.0f)
#define BUCK_CONFIG_V_DC_OUT_OFFS     (0.0f)

#define BUCK_CONFIG_V_OUT_GAIN        (3.0f * (5.83f / 0.5927f) / 4095.0f)
#define BUCK_CONFIG_V_OUT_OFFS        (0.0f)

#define BUCK_CONFIG_V_DC_IN_GAIN      (3.0f * (16.0f * 1.5f / 1.6416f) / 4095.0f)
#define BUCK_CONFIG_V_DC_IN_OFFS      (0.0f)

#define BUCK_CONFIG_V_IN_GAIN         (3.0f * (16.0f * 1.5f / 1.6588f) / 4095.0f)
#define BUCK_CONFIG_V_IN_OFFS         (0.0f)

#define BUCK_CONFIG_IL_AVG_GAIN       (3.0f * (5.9f / 3.9f) / 4095.0f / 50e-3)
#define BUCK_CONFIG_IL_AVG_OFFS       (-(2.49f / 50e-3 + 0.8603f))

#define BUCK_CONFIG_I_LIM             (10.0f)
#define BUCK_CONFIG_V_LIM             (20.0f)

#define BUCK_CONFIG_SHUTDOWN_U_DEC     0.005

#define BUCK_CONFIG_OCP_CS_ID          OCP_CS_1
#define BUCK_CONFIG_OCP_TRACE_ID       OCP_TRACE_1
//=============================================================================

#endif /* BUCK_CONFIG_H_ */
