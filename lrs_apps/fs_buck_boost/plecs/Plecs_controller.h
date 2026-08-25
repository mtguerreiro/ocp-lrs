/*
 * Header file for: lrs_fs_boost_control/Controller/Single loop/Plecs controller
 * Generated with : PLECS 4.8.10
 * Generated on   : 24 Aug 2026 14:14:45
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
   bool Memory;                     /* Plecs controller/SR Flip-flop/Memory */
   int8_t DiscreteIntegrator1_first; /* Plecs controller/Discrete Integrator1 */
   float DiscreteIntegrator1_i1_x;  /* Plecs controller/Discrete Integrator1 */
   float DiscreteIntegrator1_i2_prevU; /* Plecs controller/Discrete Integrator1 */
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
   float load_sw;                   /* Plecs controller/load_sw */
} Plecs_controller_ExternalInputs;
extern Plecs_controller_ExternalInputs Plecs_controller_U;


/* External outputs */
typedef struct
{
   float hw_outputs[7];             /* Plecs controller/hw_outputs */
} Plecs_controller_ExternalOutputs;
extern Plecs_controller_ExternalOutputs Plecs_controller_Y;


/* Block outputs */
typedef struct
{
   bool Comparator;                 /* Plecs controller/Comparator */
   float DiscreteIntegrator1;       /* Plecs controller/Discrete Integrator1 */
   float DiscreteIntegrator;        /* Plecs controller/Discrete Integrator */
   float Saturation1;               /* Plecs controller/FB lin. vo/Saturation1 */
   float Saturation;                /* Plecs controller/FB lin. vo/Saturation */
   float Saturation_1;              /* Plecs controller/FB lin io/Saturation */
   float Saturation_2;              /* Plecs controller/Saturation */
   float Zero_OrderHold[6];         /* Plecs controller/Zero-Order Hold */
   bool Memory;                     /* Plecs controller/SR Flip-flop/Memory */
   bool LogicalOperator;            /* Plecs controller/SR Flip-flop/Logical Operator */
   bool LogicalOperator4;           /* Plecs controller/SR Flip-flop/Logical Operator4 */
   float k_p_vo;                    /* Plecs controller/k_p_vo */
} Plecs_controller_BlockOutputs;
extern Plecs_controller_BlockOutputs Plecs_controller_B;

/* Entry point functions */
void Plecs_controller_initialize(float time);
void Plecs_controller_step(void);
void Plecs_controller_terminate(void);

#endif /* PLECS_HEADER_Plecs_controller_h_ */
