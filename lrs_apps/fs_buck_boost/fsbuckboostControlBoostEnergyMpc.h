
#ifndef FS_BUCK_BOOST_CONTROLLER_BOOST_ENERGY_MPC_H_
#define FS_BUCK_BOOST_CONTROLLER_BOOST_ENERGY_MPC_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"

//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================


//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcInit(void);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcSetParams(void *buffer, uint32_t size);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcGetParams(void *buffer, uint32_t size);
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostEnergyMpcReset(void);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyMpcLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostEnergyMpcGetCallbacks(void *callbacksBuffer);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* FS_BUCK_BOOST_CONTROLLER_BOOST_ENERGY_MPC_H_ */
