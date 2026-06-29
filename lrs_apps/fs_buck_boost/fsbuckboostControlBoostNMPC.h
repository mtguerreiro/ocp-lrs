
#ifndef FS_BUCK_BOOST_CONTROLLER_BOOST_NMPC_H_
#define FS_BUCK_BOOST_CONTROLLER_BOOST_NMPC_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define NX      3
#define NU      1

#define R_load       ((float)22.0f)
#define L_inductor       ((float)100e-6f)
#define Co      ((float)430e-6f)
#define V_in    ((float)6.0f)

#define I_NORM  ((float)2.5f) //I_REF
#define V_NORM  ((float)12.0f) //V_REF

#define I_ABS   ((float)4.0f)
#define TS      ((float)100e-6f)
#define RL      ((float)0.05f)
#define RCO      ((float)0.015f)

typedef struct{
    float rw;
    float io;
    float v_in;
    float qz;
}uparams_t;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCInit(void);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostNMPCRun2(
    void *meas, int32_t nmeas,
    void *refs, int32_t nrefs
);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCSetParams(void *buffer, uint32_t size);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCGetParams(void *buffer, uint32_t size);
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostNMPCReset(void);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostNMPCLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostNMPCGetCallbacks(void *callbacksBuffer);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* FS_BUCK_BOOST_CONTROLLER_BOOST_NMPC_H_ */
