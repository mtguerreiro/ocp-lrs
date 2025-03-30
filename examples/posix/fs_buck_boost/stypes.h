
#ifndef S_TYPES_H_
#define S_TYPES_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

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
}stypesMeasurements_t;

typedef struct{
}stypesSimData_t;

typedef struct{
	float u;

	float ls_sw;
	float hs_sw;

	float input_relay;
	float output_relay;
	float load_sw;

	float pwm_mode;
}stypesControl_t;

typedef struct{
}stypesControllerData_t;
//=============================================================================

#endif /* S_TYPES_H_ */
