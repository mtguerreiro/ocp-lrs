/*
 * Implementation file for: lrs_fs_boost_control/Plecs controller/Cascaded/Outer loop
 * Generated with         : PLECS 5.0.3
 * Generated on           : 3 Sep 2026 16:47:41
 */
#include "Plecs_controller_outer_loop.h"
#ifndef PLECS_HEADER_Plecs_controller_outer_loop_h_
#error The wrong header file "Plecs_controller_outer_loop.h" was included.
#error Please check your include path to see whether this file name
#error conflicts with the name of another header file.
#endif /* PLECS_HEADER_Plecs_controller_outer_loop_h_ */
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
#define PLECSRunTimeError(msg) Plecs_controller_outer_loop_errorStatus = msg
#define Plecs_controller_outer_loop_UNCONNECTED 0
static uint32_t Plecs_controller_outer_loop_tickLo;
static int32_t Plecs_controller_outer_loop_tickHi;
Plecs_controller_outer_loop_ExternalInputs Plecs_controller_outer_loop_U;
Plecs_controller_outer_loop_ExternalOutputs Plecs_controller_outer_loop_Y;
Plecs_controller_outer_loop_BlockOutputs Plecs_controller_outer_loop_B;
Plecs_controller_outer_loop_ModelStates Plecs_controller_outer_loop_X _ALIGN;
const char * Plecs_controller_outer_loop_errorStatus;
const float Plecs_controller_outer_loop_sampleTime = 5e-05f;
const char * const Plecs_controller_outer_loop_checksum =
   "34f845a9f3a8b61a2815cdfff8a3cd8222365b9d";
void Plecs_controller_outer_loop_initialize(float time)
{
   float remainder;
   Plecs_controller_outer_loop_errorStatus = NULL;
   Plecs_controller_outer_loop_tickHi =
      floor(time/(4294967296.0*Plecs_controller_outer_loop_sampleTime));
   remainder = time - Plecs_controller_outer_loop_tickHi*4294967296.0*
               Plecs_controller_outer_loop_sampleTime;
   Plecs_controller_outer_loop_tickLo = floor(
                                              remainder/Plecs_controller_outer_loop_sampleTime +
                                              .5);
   remainder -= Plecs_controller_outer_loop_tickLo*
                Plecs_controller_outer_loop_sampleTime;
   if (fabsf(remainder) > 1e-6*fabsf(time))
   {
      Plecs_controller_outer_loop_errorStatus =
         "Start time must be an integer multiple of the base sample time.";
   }
   memset(&Plecs_controller_outer_loop_X, 0,
          sizeof(Plecs_controller_outer_loop_X));

   /* Initialization for Memory : 'Outer loop/SR Flip-flop/Memory' */
   Plecs_controller_outer_loop_X.Memory = true;

   /* Initialization for Discrete Integrator : 'Outer loop/Discrete\nIntegrator' */
   Plecs_controller_outer_loop_X.DiscreteIntegrator_first = -1;
   Plecs_controller_outer_loop_X.DiscreteIntegrator_i1_x = 0;
   Plecs_controller_outer_loop_X.DiscreteIntegrator_i2_prevU = 0.f;
}

void Plecs_controller_outer_loop_step(void)
{
   if (Plecs_controller_outer_loop_errorStatus)
   {
      return;
   }

   /* Zero-Order Hold : 'Outer loop/Zero-Order\nHold'
    * incorporates
    *  Signal Inport : 'Outer loop/hw_inputs'
    */
   Plecs_controller_outer_loop_B.Zero_OrderHold[0] =
      Plecs_controller_outer_loop_U.hw_inputs[0];
   Plecs_controller_outer_loop_B.Zero_OrderHold[1] =
      Plecs_controller_outer_loop_U.hw_inputs[1];
   Plecs_controller_outer_loop_B.Zero_OrderHold[2] =
      Plecs_controller_outer_loop_U.hw_inputs[2];
   Plecs_controller_outer_loop_B.Zero_OrderHold[3] =
      Plecs_controller_outer_loop_U.hw_inputs[3];
   Plecs_controller_outer_loop_B.Zero_OrderHold[4] =
      Plecs_controller_outer_loop_U.hw_inputs[4];
   Plecs_controller_outer_loop_B.Zero_OrderHold[5] =
      Plecs_controller_outer_loop_U.hw_inputs[5];

   /* Gain : 'Outer loop/k_p_vo' */
   Plecs_controller_outer_loop_B.k_p_vo = 1.14666667f*
                                          Plecs_controller_outer_loop_B.
                                          Zero_OrderHold[4];

   /* Saturation : 'Outer loop/FB lin. vo/Saturation1'
    * incorporates
    *  Signal Inport : 'Outer loop/ref'
    */
   Plecs_controller_outer_loop_B.Saturation1 =
      Plecs_controller_outer_loop_U.ref;
   if (Plecs_controller_outer_loop_B.Saturation1 < 0.0001f)
   {
      Plecs_controller_outer_loop_B.Saturation1 = 0.0001f;
   }

   /* Saturation : 'Outer loop/FB lin. vo/Saturation'
    * incorporates
    *  Sum : 'Outer loop/FB lin. vo/Sum'
    *  Constant : 'Outer loop/FB lin. vo/Constant'
    *  Product : 'Outer loop/FB lin. vo/Product3'
    */
   Plecs_controller_outer_loop_B.Saturation = 1.f -
                                              (1.f /
                                               Plecs_controller_outer_loop_B.
                                               Saturation1 *
                                               Plecs_controller_outer_loop_B.
                                               Zero_OrderHold[3]);
   if (Plecs_controller_outer_loop_B.Saturation < 0.0001f)
   {
      Plecs_controller_outer_loop_B.Saturation = 0.0001f;
   }

   /* Memory : 'Outer loop/SR Flip-flop/Memory' */
   Plecs_controller_outer_loop_B.Memory =
      Plecs_controller_outer_loop_X.Memory;

   /* Comparator : 'Outer loop/Comparator' */
   if (Plecs_controller_outer_loop_B.Zero_OrderHold[5] >
       Plecs_controller_outer_loop_U.ref)
      Plecs_controller_outer_loop_B.Comparator = 1;
   else if (Plecs_controller_outer_loop_B.Zero_OrderHold[5] <
            Plecs_controller_outer_loop_U.ref)
      Plecs_controller_outer_loop_B.Comparator = 0;
   /* Logical Operator : 'Outer loop/SR Flip-flop/Logical\nOperator'
    * incorporates
    *  Logical Operator : 'Outer loop/SR Flip-flop/Logical\nOperator1'
    *  Logical Operator : 'Outer loop/SR Flip-flop/Logical\nOperator2'
    *  Subsystem : 'Outer loop'
    */
   Plecs_controller_outer_loop_B.LogicalOperator =
      (!Plecs_controller_outer_loop_B.Comparator) &&
      (Plecs_controller_outer_loop_UNCONNECTED ||
       Plecs_controller_outer_loop_B.Memory);

   /* Discrete Integrator : 'Outer loop/Discrete\nIntegrator' */
   if (Plecs_controller_outer_loop_X.DiscreteIntegrator_first < 0 ||
       (Plecs_controller_outer_loop_B.LogicalOperator != 0))
   {
      Plecs_controller_outer_loop_B.DiscreteIntegrator =
         Plecs_controller_outer_loop_B.k_p_vo;
   }
   else if (Plecs_controller_outer_loop_X.DiscreteIntegrator_first)
   {
      Plecs_controller_outer_loop_B.DiscreteIntegrator =
         Plecs_controller_outer_loop_X.DiscreteIntegrator_i1_x;
   }
   else
   {
      Plecs_controller_outer_loop_B.DiscreteIntegrator =
         Plecs_controller_outer_loop_X.DiscreteIntegrator_i1_x + 5e-05f*
         Plecs_controller_outer_loop_X.DiscreteIntegrator_i2_prevU;
   }

   /* Global output signals */
   Plecs_controller_outer_loop_Y.internal_ref =
      (Plecs_controller_outer_loop_B.Zero_OrderHold[2] +
       Plecs_controller_outer_loop_B.DiscreteIntegrator -
       Plecs_controller_outer_loop_B.k_p_vo) /
      Plecs_controller_outer_loop_B.Saturation;

   if (Plecs_controller_outer_loop_errorStatus)
   {
      return;
   }

   /* Update for Memory : 'Outer loop/SR Flip-flop/Memory' */
   Plecs_controller_outer_loop_X.Memory =
      Plecs_controller_outer_loop_B.LogicalOperator;

   /* Update for Discrete Integrator : 'Outer loop/Discrete\nIntegrator'
    * incorporates
    *  Gain : 'Outer loop/k_i_vo'
    *  Sum : 'Outer loop/Sum'
    *  Signal Inport : 'Outer loop/ref'
    */
   Plecs_controller_outer_loop_X.DiscreteIntegrator_first = 0;
   Plecs_controller_outer_loop_X.DiscreteIntegrator_i1_x =
      Plecs_controller_outer_loop_B.DiscreteIntegrator;
   Plecs_controller_outer_loop_X.DiscreteIntegrator_i2_prevU = 1257.43985f*
                                                               (
                                                                Plecs_controller_outer_loop_U
                                                                .ref -
                                                                Plecs_controller_outer_loop_B
                                                                .
   Zero_OrderHold[4]);
}

void Plecs_controller_outer_loop_terminate(void)
{
}
