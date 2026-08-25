/*
 * Header file for: lrs_fs_boost_control/Controller/Cascaded/Plecs controller_inner_loop
 * Generated with : PLECS 4.8.10
 * Generated on   : 24 Aug 2026 15:10:03
 */
#ifndef PLECS_HEADER_Plecs_controller_inner_loop_h_
#define PLECS_HEADER_Plecs_controller_inner_loop_h_

#include <stdbool.h>
#include <stdint.h>

/* Model floating point type */
typedef float Plecs_controller_inner_loop_FloatType;

/* Model checksum */
extern const char * const Plecs_controller_inner_loop_checksum;

/* Model error status */
extern const char * Plecs_controller_inner_loop_errorStatus;


/* Model sample time */
extern const float Plecs_controller_inner_loop_sampleTime;


/* External inputs */
typedef struct
{
   float hw_inputs[6];              /* Plecs controller_inner_loop/hw_inputs */
   float ref;                       /* Plecs controller_inner_loop/ref */
   float load_sw;                   /* Plecs controller_inner_loop/load_sw */
   float internal_ref;              /* Plecs controller_inner_loop/internal_ref */
} Plecs_controller_inner_loop_ExternalInputs;
extern Plecs_controller_inner_loop_ExternalInputs
   Plecs_controller_inner_loop_U;


/* External outputs */
typedef struct
{
   float hw_outputs[7];             /* Plecs controller_inner_loop/hw_outputs */
} Plecs_controller_inner_loop_ExternalOutputs;
extern Plecs_controller_inner_loop_ExternalOutputs
   Plecs_controller_inner_loop_Y;


/* Block outputs */
typedef struct
{
   float Saturation1;               /* Plecs controller_inner_loop/FB lin. vo/Saturation1 */
   float Saturation;                /* Plecs controller_inner_loop/Saturation */
} Plecs_controller_inner_loop_BlockOutputs;
extern Plecs_controller_inner_loop_BlockOutputs Plecs_controller_inner_loop_B;

/* Entry point functions */
void Plecs_controller_inner_loop_initialize(float time);
void Plecs_controller_inner_loop_step(void);
void Plecs_controller_inner_loop_terminate(void);

#endif /* PLECS_HEADER_Plecs_controller_inner_loop_h_ */
