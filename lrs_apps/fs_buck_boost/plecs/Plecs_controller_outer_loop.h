/*
 * Header file for: lrs_fs_boost_control/Controller/Cascaded/Plecs controller_outer_loop
 * Generated with : PLECS 4.8.10
 * Generated on   : 24 Aug 2026 15:10:07
 */
#ifndef PLECS_HEADER_Plecs_controller_outer_loop_h_
#define PLECS_HEADER_Plecs_controller_outer_loop_h_

#include <stdbool.h>
#include <stdint.h>

/* Model floating point type */
typedef float Plecs_controller_outer_loop_FloatType;

/* Model checksum */
extern const char * const Plecs_controller_outer_loop_checksum;

/* Model error status */
extern const char * Plecs_controller_outer_loop_errorStatus;


/* Model sample time */
extern const float Plecs_controller_outer_loop_sampleTime;


/*
 * Model states */
typedef struct
{
   int8_t DiscreteIntegrator_first; /* Plecs controller_outer_loop/Discrete Integrator */
   float DiscreteIntegrator_i1_x;   /* Plecs controller_outer_loop/Discrete Integrator */
   float DiscreteIntegrator_i2_prevU; /* Plecs controller_outer_loop/Discrete Integrator */
} Plecs_controller_outer_loop_ModelStates;
extern Plecs_controller_outer_loop_ModelStates Plecs_controller_outer_loop_X;


/* External inputs */
typedef struct
{
   float hw_inputs[6];              /* Plecs controller_outer_loop/hw_inputs */
   float ref;                       /* Plecs controller_outer_loop/ref */
   float load_sw;                   /* Plecs controller_outer_loop/load_sw */
} Plecs_controller_outer_loop_ExternalInputs;
extern Plecs_controller_outer_loop_ExternalInputs
   Plecs_controller_outer_loop_U;


/* External outputs */
typedef struct
{
   float internal_ref;              /* Plecs controller_outer_loop/internal_ref */
} Plecs_controller_outer_loop_ExternalOutputs;
extern Plecs_controller_outer_loop_ExternalOutputs
   Plecs_controller_outer_loop_Y;


/* Block outputs */
typedef struct
{
   float DiscreteIntegrator;        /* Plecs controller_outer_loop/Discrete Integrator */
   float Sum;                       /* Plecs controller_outer_loop/Sum */
} Plecs_controller_outer_loop_BlockOutputs;
extern Plecs_controller_outer_loop_BlockOutputs Plecs_controller_outer_loop_B;

/* Entry point functions */
void Plecs_controller_outer_loop_initialize(float time);
void Plecs_controller_outer_loop_step(void);
void Plecs_controller_outer_loop_terminate(void);

#endif /* PLECS_HEADER_Plecs_controller_outer_loop_h_ */
