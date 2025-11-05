/*
 * Header file for: lrs_fs_buck_boost/Plecs controller
 * Generated with : PLECS 4.8.6
 * Generated on   : 4 Nov 2025 11:27:16
 */
#ifndef PLECS_HEADER_Plecs_controller_h_
#define PLECS_HEADER_Plecs_controller_h_

#include <stdbool.h>
#include <stdint.h>

/* Model floating point type */
typedef float Plecs_controller_FloatType;

/* Model checksum */
extern const char * const Plecs_controller_checksum;

/* Model error status */
extern const char * Plecs_controller_errorStatus;


/* Model sample time */
extern const float Plecs_controller_sampleTime;


/* External inputs */
typedef struct
{
   float hw_inputs[6];              /* Plecs controller/hw_inputs */
   float ref;                       /* Plecs controller/ref */
   float load_sw;                   /* Plecs controller/load_sw */
} Plecs_controller_ExternalInputs;
extern Plecs_controller_ExternalInputs Plecs_controller_U;


/* External outputs */
typedef struct
{
   float hw_outputs[7];             /* Plecs controller/hw_outputs */
} Plecs_controller_ExternalOutputs;
extern Plecs_controller_ExternalOutputs Plecs_controller_Y;


/* Entry point functions */
void Plecs_controller_initialize(float time);
void Plecs_controller_step(void);
void Plecs_controller_terminate(void);

#endif /* PLECS_HEADER_Plecs_controller_h_ */
