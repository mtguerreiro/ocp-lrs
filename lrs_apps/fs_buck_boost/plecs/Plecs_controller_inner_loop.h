/*
 * Header file for: lrs_fs_boost_control/Plecs controller/Cascaded/Inner loop
 * Generated with : PLECS 5.0.3
 * Generated on   : 3 Sep 2026 16:47:45
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


/*
 * Model states */
typedef struct
{
   bool Memory;                     /* Inner loop/SR Flip-flop/Memory */
   int8_t DiscreteIntegrator1_first; /* Inner loop/Discrete Integrator1 */
   float DiscreteIntegrator1_i1_x;  /* Inner loop/Discrete Integrator1 */
   float DiscreteIntegrator1_i2_prevU; /* Inner loop/Discrete Integrator1 */
} Plecs_controller_inner_loop_ModelStates;
extern Plecs_controller_inner_loop_ModelStates Plecs_controller_inner_loop_X;


/* External inputs */
typedef struct
{
   float hw_inputs[6];              /* Inner loop/hw_inputs */
   float ref;                       /* Inner loop/ref */
   float load_sw;                   /* Inner loop/load_sw */
   float internal_ref;              /* Inner loop/internal_ref */
} Plecs_controller_inner_loop_ExternalInputs;
extern Plecs_controller_inner_loop_ExternalInputs
   Plecs_controller_inner_loop_U;


/* External outputs */
typedef struct
{
   float hw_outputs[7];             /* Inner loop/hw_outputs */
} Plecs_controller_inner_loop_ExternalOutputs;
extern Plecs_controller_inner_loop_ExternalOutputs
   Plecs_controller_inner_loop_Y;


/* Block outputs */
typedef struct
{
   bool Comparator;                 /* Inner loop/Comparator */
   float Saturation;                /* Inner loop/FB lin io/Saturation */
   float DiscreteIntegrator1;       /* Inner loop/Discrete Integrator1 */
   float Saturation_1;              /* Inner loop/Saturation */
   float Zero_OrderHold[6];         /* Inner loop/Zero-Order Hold */
   bool Memory;                     /* Inner loop/SR Flip-flop/Memory */
   bool LogicalOperator;            /* Inner loop/SR Flip-flop/Logical Operator */
} Plecs_controller_inner_loop_BlockOutputs;
extern Plecs_controller_inner_loop_BlockOutputs Plecs_controller_inner_loop_B;

/* Entry point functions */
void Plecs_controller_inner_loop_initialize(float time);
void Plecs_controller_inner_loop_step(void);
void Plecs_controller_inner_loop_terminate(void);

#endif /* PLECS_HEADER_Plecs_controller_inner_loop_h_ */
