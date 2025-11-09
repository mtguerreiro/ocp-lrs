/*
 * cukConfig.h
 *
 *  Created on: 11.09.2023
 *      Author: marco
 */

#ifndef CUK_CONFIG_H_
#define CUK_CONFIG_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
/* Hardware measurements */
typedef struct{

    float ii;
    float i1;
    float vi;
    float vi_dc;
    float v1;

    float io;
    float i2;
    float vo;
    float vo_dc;
    float v2;

}cukConfigMeasurements_t;

/* Software measurements */
typedef struct{

    float ii_filt;
    float io_filt;

    float pi;
    float po;
}cukConfigSwMeasurements_t;

typedef struct{
    float u;
}cukConfigControl_t;

typedef struct{
    float vo;
}cukConfigReferences_t;


typedef struct{
    float ii_gain;
    float ii_ofs;

    float i1_gain;
    float i1_ofs;

    float vi_gain;
    float vi_ofs;

    float vi_dc_gain;
    float vi_dc_ofs;

    float v1_gain;
    float v1_ofs;

    float io_gain;
    float io_ofs;

    float i2_gain;
    float i2_ofs;

    float vo_gain;
    float vo_ofs;

    float vo_dc_gain;
    float vo_dc_ofs;

    float v2_gain;
    float v2_ofs;
}cukConfigMeasGains_t;

#define CUK_CONFIG_TF_N2N1                  (float)((5.0f / 3.0f))
#define CUK_CONFIG_TF_N2N1_SQ               (float)(CUK_CONFIG_TF_N2N1 * CUK_CONFIG_TF_N2N1)

#define CUK_CONFIG_L_IN                     ((float)(100e-6))
#define CUK_CONFIG_L_OUT                    ((float)(150e-6))
#define CUK_CONFIG_C_C                      ((float)(9.4e-6))
#define CUK_CONFIG_C_O                      ((float)(330e-6))

#define CUK_CONFIG_I_PRIM_LIM               (10.0f)
#define CUK_CONFIG_V_PRIM_LIM               (50.0f)
#define CUK_CONFIG_I_SEC_LIM                (10.0f)
#define CUK_CONFIG_V_SEC_LIM                (50.0f)

#define CUK_CONFIG_TRACE_0_NAME_LEN         500
#define CUK_CONFIG_TRACE_0_MAX_SIGNALS      40

#define CUK_CONFIG_INPUT_BUF_SIZE           50
#define CUK_CONFIG_OUTPUT_BUF_SIZE          20
#define CUK_CONFIG_REFERENCE_BUF_SIZE       20

#define CUK_CONFIG_CS_ID                    OCP_CS_1
#define CUK_CONFIG_TRACE_ID                 OCP_TRACE_1

#ifdef OCP_LRS_ZYNQ_BUILD_CPU1
#include "zynqConfig.h"
#define CUK_CONFIG_TRACE_ADDR               ZYNQ_CONFIG_MEM_TRACE_ADR
#define CUK_CONFIG_TRACE_SIZE_BYTES         ((uint32_t)ZYNQ_CONFIG_MEM_TRACE_SIZE_MAX)
#else
#define CUK_CONFIG_TRACE_SIZE_BYTES         (16U * 1024U * 1024U)
#endif

//=============================================================================

#endif /* CUK_CONFIG_H_ */
