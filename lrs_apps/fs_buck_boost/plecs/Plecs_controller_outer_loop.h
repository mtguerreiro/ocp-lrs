/*
 * Header file for: lrs_fs_boost_control/Plecs controller/Cascaded/Outer loop
 * Generated with : PLECS 5.0.3
 * Generated on   : 3 Sep 2026 16:47:41
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
   bool Memory;                     /* Outer loop/SR Flip-flop/Memory */
   int8_t DiscreteIntegrator_first; /* Outer loop/Discrete Integrator */
   float DiscreteIntegrator_i1_x;   /* Outer loop/Discrete Integrator */
   float DiscreteIntegrator_i2_prevU; /* Outer loop/Discrete Integrator */
} Plecs_controller_outer_loop_ModelStates;
extern Plecs_controller_outer_loop_ModelStates Plecs_controller_outer_loop_X;


/* External inputs */
typedef struct
{
   float hw_inputs[6];              /* Outer loop/hw_inputs */
   float ref;                       /* Outer loop/ref */
   float load_sw;                   /* Outer loop/load_sw */
} Plecs_controller_outer_loop_ExternalInputs;
extern Plecs_controller_outer_loop_ExternalInputs
   Plecs_controller_outer_loop_U;


/* External outputs */
typedef struct
{
   float internal_ref;              /* Outer loop/internal_ref */
} Plecs_controller_outer_loop_ExternalOutputs;
extern Plecs_controller_outer_loop_ExternalOutputs
   Plecs_controller_outer_loop_Y;


/* Block outputs */
typedef struct
{
   float Saturation1;               /* Outer loop/FB lin. vo/Saturation1 */
   float Saturation;                /* Outer loop/FB lin. vo/Saturation */
   bool Comparator;                 /* Outer loop/Comparator */
   float DiscreteIntegrator;        /* Outer loop/Discrete Integrator */
   float Zero_OrderHold[6];         /* Outer loop/Zero-Order Hold */
   float k_p_vo;                    /* Outer loop/k_p_vo */
   bool Memory;                     /* Outer loop/SR Flip-flop/Memory */
   bool LogicalOperator;            /* Outer loop/SR Flip-flop/Logical Operator */
} Plecs_controller_outer_loop_BlockOutputs;
extern Plecs_controller_outer_loop_BlockOutputs Plecs_controller_outer_loop_B;

/* Entry point functions */
void Plecs_controller_outer_loop_initialize(float time);
void Plecs_controller_outer_loop_step(void);
void Plecs_controller_outer_loop_terminate(void);

#endif /* PLECS_HEADER_Plecs_controller_outer_loop_h_ */
