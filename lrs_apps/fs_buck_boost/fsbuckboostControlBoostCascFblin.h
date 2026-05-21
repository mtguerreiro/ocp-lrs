
#ifndef FS_BUCK_BOOST_CONTROLLER_BOOST_CASC_FBLIN_H_
#define FS_BUCK_BOOST_CONTROLLER_BOOST_CASC_FBLIN_H_

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
int32_t fsbuckboostControlBoostCascFblinInit(void);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostCascFblinRun2(
    void *meas, int32_t nmeas,
    void *refs, int32_t nrefs
);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinSetParams(void *buffer, uint32_t size);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinGetParams(void *buffer, uint32_t size);
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostCascFblinReset(void);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostCascFblinLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostCascFblinGetCallbacks(void *callbacksBuffer);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* FS_BUCK_BOOST_CONTROLLER_BOOST_CASC_FBLIN_H_ */
