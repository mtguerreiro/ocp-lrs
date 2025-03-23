/*
 * cukOpil.h
 *
 *  Created on: 11.09.2023
 *      Author: LRS
 */

#ifndef BOOST_OPIL_H_
#define BOOST_OPIL_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
/**
 * @brief Updates measurements data.
 *
 * The internal measurements structure is updated with the new data.
 *
 * @param meas Pointer to measurements data.
 * @param size Size of the measurements data, in bytes.
 * @return Always returns 0.
 */
int32_t boostOpilUpdateMeasurements(void *meas, int32_t size);
//-----------------------------------------------------------------------------
int32_t boostOpilGetMeasurements(void *meas);
//-----------------------------------------------------------------------------
/**
 * @brief Updates simulation data.
 *
 * The internal simulation data structure is updated with the new data.
 *
 * @param simData Pointer to simulation data.
 * @param size Size of the simulation data, in bytes.
 * @return Always returns 0.
 */
int32_t boostOpilUpdateSimData(void *simData, int32_t size);
//-----------------------------------------------------------------------------
int32_t boostOpilGetSimData(void **simData, int32_t size);
//-----------------------------------------------------------------------------
void boostOpilInitializeControl(void);
//-----------------------------------------------------------------------------
/**
 * @brief Runs the control algorithm.
 */
void boostOpilRunControl(void);
//-----------------------------------------------------------------------------
int32_t boostOpilUpdateControl(void *control, int32_t size);
//-----------------------------------------------------------------------------
/**
 * @brief Gets control data.
 *
 * The location of the structure holding the control data is stored in control.
 *
 * @param control Address of pointer to hold the address of the
 * 			      control data structure.
 * @return The size of the control data structure, in bytes.
 */
int32_t boostOpilGetControl(void **control);
//-----------------------------------------------------------------------------
void boostOpilUpdateControllerData(void *data, int32_t size);
//-----------------------------------------------------------------------------
/**
 * @brief Gets controller data.
 *
 * The location of the structure holding the controller data is stored in
 * controllerData.
 *
 * @param controllerData Address of pointer to hold the address of the
 * 			             controller data structure.
 * @return The size of the control data structure, in bytes.
 */
int32_t boostOpilGetControllerData(void **controllerData);
//-----------------------------------------------------------------------------
int32_t boostOpilProcInputs(void *inputs, void *procinputs, int32_t size);
//-----------------------------------------------------------------------------
int32_t boostOpilProcOutputs(void *outputs, void *procoutputs, int32_t size);
//-----------------------------------------------------------------------------
void boostOpilDisable(void);
//-----------------------------------------------------------------------------
void boostOpilSetPwmDuty(float duty);
//-----------------------------------------------------------------------------
float boostOpilGetPwmDuty(void);
//-----------------------------------------------------------------------------
void boostOpilSetLoadSwitch(float state);
//-----------------------------------------------------------------------------
float boostOpilGetLoadSwitch(void);
//-----------------------------------------------------------------------------
void boostOpilSetOutputSwitch(float state);
//-----------------------------------------------------------------------------
float boostOpilGetOutputSwitch(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* BOOST_OPIL_H_ */
