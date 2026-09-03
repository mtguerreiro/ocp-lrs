
#ifndef FS_BUCK_BOOST_CONTROLLER_PLECS_CASCADED_H
#define FS_BUCK_BOOST_CONTROLLER_PLECS_CASCADED_H

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
int32_t fsbuckboostControlPlecsCascadedInit(void);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsCascadedInnerLoopRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs
);
//-----------------------------------------------------------------------------
void fsbuckboostControlPlecsCascadedOuterLoopRun(
    void *meas, int32_t nmeas,
    void *refs, int32_t nrefs
);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsCascadedSetParams(void *buffer, uint32_t size);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsCascadedGetParams(void *buffer, uint32_t size);
//-----------------------------------------------------------------------------
void fsbuckboostControlPlecsCascadedReset(void);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsCascadedFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsCascadedLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
void fsbuckboostControlPlecsCascadedGetCallbacks(void *callbacksBuffer);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* FS_BUCK_BOOST_CONTROLLER_PLECS_CASCADED_H_ */
