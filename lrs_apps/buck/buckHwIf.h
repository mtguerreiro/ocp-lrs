/*
 * buckHwIf.h
 *
 */

#ifndef BUCK_HW_IF_H_
#define BUCK_HW_IF_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
    /**
     * Command
     *  ---------------------------------------------
     *  | SET PWM ENABLE (uint32) | ENABLE (uint32) |
     *  ---------------------------------------------
     *
     * Response
     * No response
     *
     */
    BUCK_HW_IF_SET_PWM_ENABLE,

    /**
     * Command
     *  ---------------------------
     *  | GET PWM ENABLE (uint32) |
     *  ---------------------------
     *
     * Response
     *  -------------------
     *  | ENABLE (uint32) |
     *  -------------------
     */
    BUCK_HW_IF_GET_PWM_ENABLE,

    /**
     * Command
     *  ---------------------------------------------------
     *  | SET PWM FREQUENCY (uint32) | FREQUENCY (uint32) |
     *  ---------------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - FREQUENCY is given in Hz. Note that it may not be possible to generate
     *   arbitrary frequencies.
     */
    BUCK_HW_IF_SET_PWM_FREQ,

    /**
     * Command
     *  ------------------------------
     *  | GET PWM FREQUENCY (uint32) |
     *  ------------------------------
     *
     * Response
     *  ----------------------
     *  | FREQUENCY (uint32) |
     *  ----------------------
     *
     * Notes
     * - FREQUENCY is given in Hz.
     */
    BUCK_HW_IF_GET_PWM_FREQ,

    /**
     * Command
     *  ----------------------------------------
     *  | SET PWM DUTY (uint32) | DUTY (float) |
     *  ----------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - DUTY is given as a floating point value between 0 and 1.
     */
    BUCK_HW_IF_SET_PWM_DUTY,

    /**
     * Command
     *  -------------------------
     *  | GET PWM DUTY (uint32) |
     *  -------------------------
     *
     * Response
     *  ----------------
     *  | DUTY (float) |
     *  ----------------
     *
     * Notes
     * - DUTY is given as a value between 0 and 1.
     */
    BUCK_HW_IF_GET_PWM_DUTY,

    /**
     * Command
     *  ---------------------------------------------------
     *  | SET PWM DEAD TIME (uint32) | DEAD TIME (uint32) |
     *  ---------------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - DEAD TIME is given in seconds.
     */
    BUCK_HW_IF_SET_PWM_DEAD_TIME,

    /**
     * Command
     *  -----------------------------
     *  | GET PWM DEAD TIME(uint32) |
     *  -----------------------------
     *
     * Response
     *  ---------------------
     *  | DEAD TIME (float) |
     *  ---------------------
     *
     * Notes
     * - PWM dead time, in seconds.
     */
    BUCK_HW_IF_GET_PWM_DEAD_TIME,

    /**
     * Command
     *  ---------------------------------------------
     *  | SET INPUT RELAY (uint32) | STATE (uint32) |
     *  ---------------------------------------------
     *
     * Response
     * No response
     *
     */
    BUCK_HW_IF_SET_INPUT_RELAY,

    /**
     * Command
     *  ----------------------------
     *  | GET INPUT RELAY (uint32) |
     *  ----------------------------
     *
     * Response
     *  ------------------------
     *  | INPUT RELAY (uint32) |
     *  ------------------------
     *
     */
    BUCK_HW_IF_GET_INPUT_RELAY,

    /**
     * Command
     *  ----------------------------------------------
     *  | SET OUTPUT RELAY (uint32) | STATE (uint32) |
     *  ----------------------------------------------
     *
     * Response
     * No response
     *
     */
    BUCK_HW_IF_SET_OUTPUT_RELAY,

    /**
     * Command
     *  -----------------------------
     *  | GET OUTPUT RELAY (uint32) |
     *  -----------------------------
     *
     * Response
     *  -------------------------
     *  | OUTPUT RELAY (uint32) |
     *  -------------------------
     *
     */
    BUCK_HW_IF_GET_OUTPUT_RELAY,

    /**
     * Command
     *  ---------------------------------------------
     *  | SET MEAS GAINS (uint32) | GAINS (N bytes) |
     *  ---------------------------------------------
     *
     * Response
     * No response
     *
     */
    BUCK_HW_IF_SET_MEAS_GAINS,

    /**
     * Command
     *  ---------------------------
     *  | GET MEAS GAINS (uint32) |
     *  ---------------------------
     *
     * Response
     *  -------------------
     *  | GAINS (N bytes) |
     *  -------------------
     *
     */
    BUCK_HW_IF_GET_MEAS_GAINS,

    /**
     * Command
     *  -------------------------
     *  | CLEAR STATUS (uint32) |
     *  -------------------------
     *
     * Response
     * No response
     *
     */
    BUCK_HW_IF_CLEAR_STATUS,

    /**
     * Command
     *  -----------------------
     *  | GET STATUS (uint32) |
     *  -----------------------
     *
     * Response
     *  -------------------
     *  | STATUS (uint32) |
     *  -------------------
     *
     */
    BUCK_HW_IF_GET_STATUS,

    BUCK_HW_IF_END
}buckHwIfCommands_t;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckHwIfInitialize(void);
//-----------------------------------------------------------------------------
int32_t buckHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* BUCK_HW_IF_H_ */
