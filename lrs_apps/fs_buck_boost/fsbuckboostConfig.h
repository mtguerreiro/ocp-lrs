
#ifndef FS_BUCK_BOOST_CONFIG_H_
#define FS_BUCK_BOOST_CONFIG_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    
    float ii;
    float il;
    float io;
    
    float v_in;
    float v_dc_out;
    float v_out;    
}fsbuckboostConfigMeasurements_t;

typedef struct{
    float u;
}fsbuckboostConfigControl_t;

typedef struct{
    float v_out;
}fsbuckboostConfigReferences_t;

typedef struct{

    float ii_gain;
    float ii_ofs;

    float il_gain;
    float il_ofs;

    float io_gain;
    float io_ofs;

    float v_in_gain;
    float v_in_ofs;
    
    float v_dc_out_gain;
    float v_dc_out_ofs;

    float v_out_gain;
    float v_out_ofs;
}fsbuckboostConfigMeasGains_t;

#define FS_BUCK_BOOST_CFG_II_GAIN       (0.012080510642777674f)
#define FS_BUCK_BOOST_CFG_II_OFFS       (-24.80068559304638f)

#define FS_BUCK_BOOST_CFG_IL_GAIN       (0.012452883241412915f)
#define FS_BUCK_BOOST_CFG_IL_OFFS       (-25.44104518340359f)

#define FS_BUCK_BOOST_CFG_IO_GAIN       (0.011751354084716911f)
#define FS_BUCK_BOOST_CFG_IO_OFFS       (-23.73712549633501f)

#define FS_BUCK_BOOST_CFG_V_IN_GAIN     (0.014789272601660538f)
#define FS_BUCK_BOOST_CFG_V_IN_OFFS     (-0.008375517562761644f)

#define FS_BUCK_BOOST_CFG_V_DC_OUT_GAIN (0.01464151004446206f)
#define FS_BUCK_BOOST_CFG_V_DC_OUT_OFFS (-0.012438849450212095f)

#define FS_BUCK_BOOST_CFG_V_OUT_GAIN    (0.014724696702574171f)
#define FS_BUCK_BOOST_CFG_V_OUT_OFFS    (-0.007491753705390119f)

#define FS_BUCK_BOOST_CONFIG_I_LIM      22.5f
#define FS_BUCK_BOOST_CONFIG_V_LIM      50.0f

#ifdef SOC_CPU1
#define FS_BUCK_BOOST_CONFIG_TRACE_ADDR             ZYNQ_CONFIG_MEM_TRACE_ADR
#define FS_BUCK_BOOST_CONFIG_TRACE_SIZE             ZYNQ_CONFIG_MEM_TRACE_SIZE_MAX
#endif

#define FS_BUCK_BOOST_CONFIG_TRACE_0_NAME_LEN       500
#define FS_BUCK_BOOST_CONFIG_TRACE_0_MAX_SIGNALS    40

#define FS_BUCK_BOOST_CONFIG_INPUT_BUF_SIZE         50
#define FS_BUCK_BOOST_CONFIG_OUTPUT_BUG_SIZE        20
#define FS_BUCK_BOOST_CONFIG_REFERENCE_BUF_SIZE     20

#define FS_BUCK_BOOST_CONFIG_CS_ID                  OCP_CS_1
#define FS_BUCK_BOOST_CONFIG_TRACE_ID               OCP_TRACE_1
//=============================================================================

#endif /* FS_BUCK_BOOST_CONFIG_H_ */
