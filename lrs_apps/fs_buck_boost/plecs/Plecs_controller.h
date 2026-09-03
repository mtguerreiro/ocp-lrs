/*
 * Header file for: lrs_fs_boost_control/Plecs controller/Single loop/Controller
 * Generated with : PLECS 5.0.3
 * Generated on   : 3 Sep 2026 16:47:33
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
   bool Memory;                     /* Controller/SR Flip-flop/Memory */
   int8_t DiscreteIntegrator_first; /* Controller/Discrete Integrator */
   float DiscreteIntegrator_i1_x;   /* Controller/Discrete Integrator */
   float DiscreteIntegrator_i2_prevU; /* Controller/Discrete Integrator */
   int8_t DiscreteIntegrator1_first; /* Controller/Discrete Integrator1 */
   float DiscreteIntegrator1_i1_x;  /* Controller/Discrete Integrator1 */
   float DiscreteIntegrator1_i2_prevU; /* Controller/Discrete Integrator1 */
} Plecs_controller_ModelStates;
extern Plecs_controller_ModelStates Plecs_controller_X;


/* External inputs */
typedef struct
{
   float hw_inputs[6];              /* Controller/hw_inputs */
   float ref;                       /* Controller/ref */
   float load_sw;                   /* Controller/load_sw */
} Plecs_controller_ExternalInputs;
extern Plecs_controller_ExternalInputs Plecs_controller_U;


/* External outputs */
typedef struct
{
   float hw_outputs[7];             /* Controller/hw_outputs */
} Plecs_controller_ExternalOutputs;
extern Plecs_controller_ExternalOutputs Plecs_controller_Y;


/* Block outputs */
typedef struct
{
   float Saturation;                /* Controller/FB lin io/Saturation */
   float Saturation1;               /* Controller/FB lin. vo/Saturation1 */
   float Saturation_1;              /* Controller/FB lin. vo/Saturation */
   bool Comparator;                 /* Controller/Comparator */
   float DiscreteIntegrator;        /* Controller/Discrete Integrator */
   float DiscreteIntegrator1;       /* Controller/Discrete Integrator1 */
   float Saturation_2;              /* Controller/Saturation */
   float Zero_OrderHold[6];         /* Controller/Zero-Order Hold */
   float k_p_vo;                    /* Controller/k_p_vo */
   bool Memory;                     /* Controller/SR Flip-flop/Memory */
   bool LogicalOperator;            /* Controller/SR Flip-flop/Logical Operator */
} Plecs_controller_BlockOutputs;
extern Plecs_controller_BlockOutputs Plecs_controller_B;

/* Entry point functions */
void Plecs_controller_initialize(float time);
void Plecs_controller_step(void);
void Plecs_controller_terminate(void);

#endif /* PLECS_HEADER_Plecs_controller_h_ */
