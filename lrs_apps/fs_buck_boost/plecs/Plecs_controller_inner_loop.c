/*
 * Implementation file for: lrs_fs_boost_control/Plecs controller/Cascaded/Inner loop
 * Generated with         : PLECS 5.0.3
 * Generated on           : 3 Sep 2026 16:47:45
 */
#include "Plecs_controller_inner_loop.h"
#ifndef PLECS_HEADER_Plecs_controller_inner_loop_h_
#error The wrong header file "Plecs_controller_inner_loop.h" was included.
#error Please check your include path to see whether this file name
#error conflicts with the name of another header file.
#endif /* PLECS_HEADER_Plecs_controller_inner_loop_h_ */
#if defined(__GNUC__) && (__GNUC__ > 4)
#   define _ALIGNMENT 16
#   define _RESTRICT __restrict
#   define _ALIGN __attribute__((aligned(_ALIGNMENT)))
#   if defined(__clang__)
#      if __has_builtin(__builtin_assume_aligned)
#         define _ASSUME_ALIGNED(a) __builtin_assume_aligned(a, _ALIGNMENT)
#      else
#         define _ASSUME_ALIGNED(a) a
#      endif
#   else
#      define _ASSUME_ALIGNED(a) __builtin_assume_aligned(a, _ALIGNMENT)
#   endif
#else
#   ifndef _RESTRICT
#      define _RESTRICT
#   endif
#   ifndef _ALIGN
#      define _ALIGN
#   endif
#   ifndef _ASSUME_ALIGNED
#      define _ASSUME_ALIGNED(a) a
#   endif
#endif
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#define PLECSRunTimeError(msg) Plecs_controller_inner_loop_errorStatus = msg
#define Plecs_controller_inner_loop_UNCONNECTED 0
static uint32_t Plecs_controller_inner_loop_tickLo;
static int32_t Plecs_controller_inner_loop_tickHi;
Plecs_controller_inner_loop_ExternalInputs Plecs_controller_inner_loop_U;
Plecs_controller_inner_loop_ExternalOutputs Plecs_controller_inner_loop_Y;
Plecs_controller_inner_loop_BlockOutputs Plecs_controller_inner_loop_B;
Plecs_controller_inner_loop_ModelStates Plecs_controller_inner_loop_X _ALIGN;
const char * Plecs_controller_inner_loop_errorStatus;
const float Plecs_controller_inner_loop_sampleTime = 1e-05f;
const char * const Plecs_controller_inner_loop_checksum =
   "b15af8b328a72890c8e94cb5656010ffb83d1fb2";
void Plecs_controller_inner_loop_initialize(float time)
{
   float remainder;
   Plecs_controller_inner_loop_errorStatus = NULL;
   Plecs_controller_inner_loop_tickHi =
      floor(time/(4294967296.0*Plecs_controller_inner_loop_sampleTime));
   remainder = time - Plecs_controller_inner_loop_tickHi*4294967296.0*
               Plecs_controller_inner_loop_sampleTime;
   Plecs_controller_inner_loop_tickLo = floor(
                                              remainder/Plecs_controller_inner_loop_sampleTime +
                                              .5);
   remainder -= Plecs_controller_inner_loop_tickLo*
                Plecs_controller_inner_loop_sampleTime;
   if (fabsf(remainder) > 1e-6*fabsf(time))
   {
      Plecs_controller_inner_loop_errorStatus =
         "Start time must be an integer multiple of the base sample time.";
   }
   memset(&Plecs_controller_inner_loop_X, 0,
          sizeof(Plecs_controller_inner_loop_X));

   /* Initialization for Memory : 'Inner loop/SR Flip-flop/Memory' */
   Plecs_controller_inner_loop_X.Memory = true;

   /* Initialization for Discrete Integrator : 'Inner loop/Discrete\nIntegrator1' */
   Plecs_controller_inner_loop_X.DiscreteIntegrator1_first = -1;
   Plecs_controller_inner_loop_X.DiscreteIntegrator1_i1_x = 0;
   Plecs_controller_inner_loop_X.DiscreteIntegrator1_i2_prevU = 0.f;
}

void Plecs_controller_inner_loop_step(void)
{
   if (Plecs_controller_inner_loop_errorStatus)
   {
      return;
   }

   /* Zero-Order Hold : 'Inner loop/Zero-Order\nHold'
    * incorporates
    *  Signal Inport : 'Inner loop/hw_inputs'
    */
   Plecs_controller_inner_loop_B.Zero_OrderHold[0] =
      Plecs_controller_inner_loop_U.hw_inputs[0];
   Plecs_controller_inner_loop_B.Zero_OrderHold[1] =
      Plecs_controller_inner_loop_U.hw_inputs[1];
   Plecs_controller_inner_loop_B.Zero_OrderHold[2] =
      Plecs_controller_inner_loop_U.hw_inputs[2];
   Plecs_controller_inner_loop_B.Zero_OrderHold[3] =
      Plecs_controller_inner_loop_U.hw_inputs[3];
   Plecs_controller_inner_loop_B.Zero_OrderHold[4] =
      Plecs_controller_inner_loop_U.hw_inputs[4];
   Plecs_controller_inner_loop_B.Zero_OrderHold[5] =
      Plecs_controller_inner_loop_U.hw_inputs[5];

   /* Memory : 'Inner loop/SR Flip-flop/Memory' */
   Plecs_controller_inner_loop_B.Memory =
      Plecs_controller_inner_loop_X.Memory;

   /* Comparator : 'Inner loop/Comparator' */
   if (Plecs_controller_inner_loop_B.Zero_OrderHold[5] >
       Plecs_controller_inner_loop_U.ref)
      Plecs_controller_inner_loop_B.Comparator = 1;
   else if (Plecs_controller_inner_loop_B.Zero_OrderHold[5] <
            Plecs_controller_inner_loop_U.ref)
      Plecs_controller_inner_loop_B.Comparator = 0;
   /* Saturation : 'Inner loop/FB lin io/Saturation' */
   Plecs_controller_inner_loop_B.Saturation =
      Plecs_controller_inner_loop_B.Zero_OrderHold[5];
   if (Plecs_controller_inner_loop_B.Saturation < 0.1f)
   {
      Plecs_controller_inner_loop_B.Saturation = 0.1f;
   }

   /* Logical Operator : 'Inner loop/SR Flip-flop/Logical\nOperator'
    * incorporates
    *  Logical Operator : 'Inner loop/SR Flip-flop/Logical\nOperator1'
    *  Logical Operator : 'Inner loop/SR Flip-flop/Logical\nOperator2'
    *  Subsystem : 'Inner loop'
    */
   Plecs_controller_inner_loop_B.LogicalOperator =
      (!Plecs_controller_inner_loop_B.Comparator) &&
      (Plecs_controller_inner_loop_UNCONNECTED ||
       Plecs_controller_inner_loop_B.Memory);

   /* Discrete Integrator : 'Inner loop/Discrete\nIntegrator1'
    * incorporates
    *  Logical Operator : 'Inner loop/SR Flip-flop/Logical\nOperator4'
    *  Logical Operator : 'Inner loop/SR Flip-flop/Logical\nOperator3'
    *  Subsystem : 'Inner loop'
    */
   if (Plecs_controller_inner_loop_X.DiscreteIntegrator1_first < 0 ||
       (!(Plecs_controller_inner_loop_B.LogicalOperator ||
          (Plecs_controller_inner_loop_UNCONNECTED &&
           Plecs_controller_inner_loop_B.Comparator)) != 0))
   {
      Plecs_controller_inner_loop_B.DiscreteIntegrator1 = 0.f;
   }
   else if (Plecs_controller_inner_loop_X.DiscreteIntegrator1_first)
   {
      Plecs_controller_inner_loop_B.DiscreteIntegrator1 =
         Plecs_controller_inner_loop_X.DiscreteIntegrator1_i1_x;
   }
   else
   {
      Plecs_controller_inner_loop_B.DiscreteIntegrator1 =
         Plecs_controller_inner_loop_X.DiscreteIntegrator1_i1_x + 1e-05f*
         Plecs_controller_inner_loop_X.DiscreteIntegrator1_i2_prevU;
   }

   /* Saturation : 'Inner loop/Saturation'
    * incorporates
    *  Signal Switch : 'Inner loop/Switch'
    *  Gain : 'Inner loop/Gain'
    *  Sum : 'Inner loop/FB lin io/Sum'
    *  Product : 'Inner loop/FB lin io/Product1'
    *  Sum : 'Inner loop/FB lin io/Sum1'
    *  Gain : 'Inner loop/k_p_il'
    *  Sum : 'Inner loop/Sum4'
    *  Signal Inport : 'Inner loop/internal_ref'
    *  Constant : 'Inner loop/FB lin io/one'
    */
   Plecs_controller_inner_loop_B.Saturation_1 =
      (Plecs_controller_inner_loop_B.LogicalOperator != false) ? (100.f*
                                                                  Plecs_controller_inner_loop_B
                                                                  .
                                                                  DiscreteIntegrator1)
   : (-((-(0.666666667f*
                                                                                               (
                                                                                                 Plecs_controller_inner_loop_U
                                                                                                 .
   internal_ref -
                                                                                                 Plecs_controller_inner_loop_B
                                                                                                 .
   Zero_OrderHold[1])) +
                                                                                             Plecs_controller_inner_loop_B
                                                                                             .
                                                                                             Zero_OrderHold
   [3]) /
                                                                                            Plecs_controller_inner_loop_B
                                                                                            .
                                                                                            Saturation)
   + 1.f);
   if (Plecs_controller_inner_loop_B.Saturation_1 > 0.75f)
   {
      Plecs_controller_inner_loop_B.Saturation_1 = 0.75f;
   }
   else if (Plecs_controller_inner_loop_B.Saturation_1 < 0.f)
   {
      Plecs_controller_inner_loop_B.Saturation_1 = 0.f;
   }

   /* Global output signals */
   Plecs_controller_inner_loop_Y.hw_outputs[0] =
      Plecs_controller_inner_loop_B.Saturation_1;
   Plecs_controller_inner_loop_Y.hw_outputs[1] = 0.f;
   Plecs_controller_inner_loop_Y.hw_outputs[2] = 1.f;
   Plecs_controller_inner_loop_Y.hw_outputs[3] = 1.f;
   Plecs_controller_inner_loop_Y.hw_outputs[4] = 1.f;
   Plecs_controller_inner_loop_Y.hw_outputs[5] =
      Plecs_controller_inner_loop_U.load_sw;
   Plecs_controller_inner_loop_Y.hw_outputs[6] = 1.f;

   if (Plecs_controller_inner_loop_errorStatus)
   {
      return;
   }

   /* Update for Memory : 'Inner loop/SR Flip-flop/Memory' */
   Plecs_controller_inner_loop_X.Memory =
      Plecs_controller_inner_loop_B.LogicalOperator;

   /* Update for Discrete Integrator : 'Inner loop/Discrete\nIntegrator1' */
   Plecs_controller_inner_loop_X.DiscreteIntegrator1_first = 0;
   Plecs_controller_inner_loop_X.DiscreteIntegrator1_i1_x =
      Plecs_controller_inner_loop_B.DiscreteIntegrator1;
   Plecs_controller_inner_loop_X.DiscreteIntegrator1_i2_prevU =
      Plecs_controller_inner_loop_B.LogicalOperator;
}

void Plecs_controller_inner_loop_terminate(void)
{
}
