/*
 * Header file for: lrs_buck_control/Plecs controller
 * Generated with : PLECS 4.8.10
 * Generated on   : 10 Nov 2025 15:14:15
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


/*
 * Model states */
typedef struct
{
   int8_t DiscreteIntegrator_first; /* Plecs controller/Discrete Integrator */
   float DiscreteIntegrator_i1_x;   /* Plecs controller/Discrete Integrator */
   float DiscreteIntegrator_i2_prevU; /* Plecs controller/Discrete Integrator */
} Plecs_controller_ModelStates;
extern Plecs_controller_ModelStates Plecs_controller_X;


/* External inputs */
typedef struct
{
   float hw_inputs[6];              /* Plecs controller/hw_inputs */
   float ref;                       /* Plecs controller/ref */
   float output_relay;              /* Plecs controller/output_relay */
} Plecs_controller_ExternalInputs;
extern Plecs_controller_ExternalInputs Plecs_controller_U;


/* External outputs */
typedef struct
{
   float hw_outputs[3];             /* Plecs controller/hw_outputs */
} Plecs_controller_ExternalOutputs;
extern Plecs_controller_ExternalOutputs Plecs_controller_Y;


/* Block outputs */
typedef struct
{
   float DiscreteIntegrator;        /* Plecs controller/Discrete Integrator */
   float Saturation;                /* Plecs controller/Saturation */
   float Zero_OrderHold[6];         /* Plecs controller/Zero-Order Hold */
} Plecs_controller_BlockOutputs;
extern Plecs_controller_BlockOutputs Plecs_controller_B;

/* Entry point functions */
void Plecs_controller_initialize(float time);
void Plecs_controller_step(void);
void Plecs_controller_terminate(void);

#endif /* PLECS_HEADER_Plecs_controller_h_ */
