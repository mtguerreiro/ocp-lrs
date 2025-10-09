/*
 * Header file for: lrs_fs_buck_boost/Buck controller
 * Generated with : PLECS 4.8.10
 * Generated on   : 8 Oct 2025 19:11:08
 */
#ifndef PLECS_HEADER_Buck_controller_h_
#define PLECS_HEADER_Buck_controller_h_

#include <stdbool.h>
#include <stdint.h>

/* Model floating point type */
typedef float Buck_controller_FloatType;

/* Model checksum */
extern const char * const Buck_controller_checksum;

/* Model error status */
extern const char * Buck_controller_errorStatus;


/* Model sample time */
extern const float Buck_controller_sampleTime;


/*
 * Model states */
typedef struct
{
   int8_t DiscreteIntegrator_first; /* Buck controller/Discrete Integrator */
   float DiscreteIntegrator_i1_x;   /* Buck controller/Discrete Integrator */
   float DiscreteIntegrator_i2_prevU; /* Buck controller/Discrete Integrator */
} Buck_controller_ModelStates;
extern Buck_controller_ModelStates Buck_controller_X;


/* External inputs */
typedef struct
{
   float hw_inputs[6];              /* Buck controller/hw_inputs */
   float ref;                       /* Buck controller/ref */
   float load_sw;                   /* Buck controller/load_sw */
} Buck_controller_ExternalInputs;
extern Buck_controller_ExternalInputs Buck_controller_U;


/* External outputs */
typedef struct
{
   float hw_outputs[7];             /* Buck controller/hw_outputs */
} Buck_controller_ExternalOutputs;
extern Buck_controller_ExternalOutputs Buck_controller_Y;


/* Block outputs */
typedef struct
{
   float DiscreteIntegrator;        /* Buck controller/Discrete Integrator */
   float Saturation;                /* Buck controller/Saturation */
   float Zero_OrderHold[6];         /* Buck controller/Zero-Order Hold */
} Buck_controller_BlockOutputs;
extern Buck_controller_BlockOutputs Buck_controller_B;

/* Entry point functions */
void Buck_controller_initialize(float time);
void Buck_controller_step(void);
void Buck_controller_terminate(void);

#endif /* PLECS_HEADER_Buck_controller_h_ */
