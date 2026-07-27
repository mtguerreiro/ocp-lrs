
#ifndef FS_BUCK_BOOST_CONTROLLER_PLECS_2FREQ_H
#define FS_BUCK_BOOST_CONTROLLER_PLECS_2FREQ_H

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
int32_t fsbuckboostControl2FreqPlecsInit(void);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlPlecsInnerLoopRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);

void fsbuckboostControlPlecsOuterLoopRun(
    void *meas, int32_t nmeas,
    void *refs, int32_t nrefs
);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControl2FreqPlecsSetParams(void *buffer, uint32_t size);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControl2FreqPlecsGetParams(void *buffer, uint32_t size);
//-----------------------------------------------------------------------------
void fsbuckboostControl2FreqPlecsReset(void);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControl2FreqPlecsFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t fsbuckboostControl2FreqPlecsLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
void fsbuckboostControlBoost2FREQGetCallbacks(void *callbacksBuffer);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* FS_BUCK_BOOST_CONTROLLER_PLECS_2FREQ_H_ */
