/*
 * Implementation file for: lrs_fs_boost_control/Controller/single_block_control/Plecs controller
 * Generated with         : PLECS 4.8.10
 * Generated on           : 24 Jul 2026 15:42:49
 */
#include "Plecs_controller.h"
#ifndef PLECS_HEADER_Plecs_controller_h_
#error The wrong header file "Plecs_controller.h" was included. Please
#error check your include path to see whether this file name conflicts with
#error the name of another header file.
#endif /* PLECS_HEADER_Plecs_controller_h_ */
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
#define PLECSRunTimeError(msg) Plecs_controller_errorStatus = msg
#define Plecs_controller_UNCONNECTED 0
static uint32_t Plecs_controller_tickLo;
static int32_t Plecs_controller_tickHi;
Plecs_controller_ExternalInputs Plecs_controller_U;
Plecs_controller_ExternalOutputs Plecs_controller_Y;
Plecs_controller_BlockOutputs Plecs_controller_B;
Plecs_controller_ModelStates Plecs_controller_X _ALIGN;
const char * Plecs_controller_errorStatus;
const float Plecs_controller_sampleTime = 1e-05f;
const char * const Plecs_controller_checksum =
   "8cbda0fe7397e6a8232c15d3a4409eb4d875b000";
void Plecs_controller_initialize(float time)
{
   float remainder;
   Plecs_controller_errorStatus = NULL;
   Plecs_controller_tickHi =
      floor(time/(4294967296.0*Plecs_controller_sampleTime));
   remainder = time - Plecs_controller_tickHi*4294967296.0*
               Plecs_controller_sampleTime;
   Plecs_controller_tickLo =
      floor(remainder/Plecs_controller_sampleTime + .5);
   remainder -= Plecs_controller_tickLo*Plecs_controller_sampleTime;
   if (fabsf(remainder) > 1e-6*fabsf(time))
   {
      Plecs_controller_errorStatus =
         "Start time must be an integer multiple of the base sample time.";
   }
   memset(&Plecs_controller_X, 0, sizeof(Plecs_controller_X));

   /* Initialization for Memory : 'Plecs\ncontroller/SR Flip-flop/Memory' */
   Plecs_controller_X.Memory = true;

   /* Initialization for Discrete Integrator : 'Plecs\ncontroller/Discrete\nIntegrator1' */
   Plecs_controller_X.DiscreteIntegrator1_first = -1;
   Plecs_controller_X.DiscreteIntegrator1_i1_x = 0;
   Plecs_controller_X.DiscreteIntegrator1_i2_prevU = 0.f;

   /* Initialization for Discrete Integrator : 'Plecs\ncontroller/Discrete\nIntegrator' */
   Plecs_controller_X.DiscreteIntegrator_first = -1;
   Plecs_controller_X.DiscreteIntegrator_i1_x = 0;
   Plecs_controller_X.DiscreteIntegrator_i2_prevU = 0.f;
}

void Plecs_controller_step(void)
{
   if (Plecs_controller_errorStatus)
   {
      return;
   }

   /* Zero-Order Hold : 'Plecs\ncontroller/Zero-Order\nHold'
    * incorporates
    *  Signal Inport : 'Plecs\ncontroller/hw_inputs'
    */
   Plecs_controller_B.Zero_OrderHold[0] = Plecs_controller_U.hw_inputs[0];
   Plecs_controller_B.Zero_OrderHold[1] = Plecs_controller_U.hw_inputs[1];
   Plecs_controller_B.Zero_OrderHold[2] = Plecs_controller_U.hw_inputs[2];
   Plecs_controller_B.Zero_OrderHold[3] = Plecs_controller_U.hw_inputs[3];
   Plecs_controller_B.Zero_OrderHold[4] = Plecs_controller_U.hw_inputs[4];
   Plecs_controller_B.Zero_OrderHold[5] = Plecs_controller_U.hw_inputs[5];

   /* Comparator : 'Plecs\ncontroller/Comparator' */
   if (Plecs_controller_B.Zero_OrderHold[5] > Plecs_controller_U.ref)
      Plecs_controller_B.Comparator = 1;
   else if (Plecs_controller_B.Zero_OrderHold[5] < Plecs_controller_U.ref)
      Plecs_controller_B.Comparator = 0;
   /* Memory : 'Plecs\ncontroller/SR Flip-flop/Memory' */
   Plecs_controller_B.Memory = Plecs_controller_X.Memory;

   /* Logical Operator : 'Plecs\ncontroller/SR Flip-flop/Logical\nOperator'
    * incorporates
    *  Logical Operator : 'Plecs\ncontroller/SR Flip-flop/Logical\nOperator1'
    *  Logical Operator : 'Plecs\ncontroller/SR Flip-flop/Logical\nOperator2'
    *  Subsystem : 'Plecs\ncontroller'
    */
   Plecs_controller_B.LogicalOperator = (!Plecs_controller_B.Comparator) &&
                                        (Plecs_controller_UNCONNECTED ||
                                         Plecs_controller_B.Memory);

   /* Logical Operator : 'Plecs\ncontroller/SR Flip-flop/Logical\nOperator4'
    * incorporates
    *  Logical Operator : 'Plecs\ncontroller/SR Flip-flop/Logical\nOperator3'
    *  Subsystem : 'Plecs\ncontroller'
    */
   Plecs_controller_B.LogicalOperator4 =
      !(Plecs_controller_B.LogicalOperator ||
        (Plecs_controller_UNCONNECTED && Plecs_controller_B.Comparator));

   /* Discrete Integrator : 'Plecs\ncontroller/Discrete\nIntegrator1' */
   if (Plecs_controller_X.DiscreteIntegrator1_first < 0 ||
       (Plecs_controller_B.LogicalOperator4 != 0))
   {
      Plecs_controller_B.DiscreteIntegrator1 = 0.f;
   }
   else if (Plecs_controller_X.DiscreteIntegrator1_first)
   {
      Plecs_controller_B.DiscreteIntegrator1 =
         Plecs_controller_X.DiscreteIntegrator1_i1_x;
   }
   else
   {
      Plecs_controller_B.DiscreteIntegrator1 =
         Plecs_controller_X.DiscreteIntegrator1_i1_x + 1e-05f*
         Plecs_controller_X.DiscreteIntegrator1_i2_prevU;
   }

   /* Gain : 'Plecs\ncontroller/k_p_vo' */
   Plecs_controller_B.k_p_vo = 0.4f*Plecs_controller_B.Zero_OrderHold[4];

   /* Discrete Integrator : 'Plecs\ncontroller/Discrete\nIntegrator' */
   if (Plecs_controller_X.DiscreteIntegrator_first < 0 ||
       (Plecs_controller_B.LogicalOperator != 0))
   {
      Plecs_controller_B.DiscreteIntegrator = Plecs_controller_B.k_p_vo;
   }
   else if (Plecs_controller_X.DiscreteIntegrator_first)
   {
      Plecs_controller_B.DiscreteIntegrator =
         Plecs_controller_X.DiscreteIntegrator_i1_x;
   }
   else
   {
      Plecs_controller_B.DiscreteIntegrator =
         Plecs_controller_X.DiscreteIntegrator_i1_x + 1e-05f*
         Plecs_controller_X.DiscreteIntegrator_i2_prevU;
   }

   /* Saturation : 'Plecs\ncontroller/FB lin. vo/Saturation1'
    * incorporates
    *  Signal Inport : 'Plecs\ncontroller/ref'
    */
   Plecs_controller_B.Saturation1 = Plecs_controller_U.ref;
   if (Plecs_controller_B.Saturation1 < 0.0001f)
   {
      Plecs_controller_B.Saturation1 = 0.0001f;
   }

   /* Saturation : 'Plecs\ncontroller/FB lin. vo/Saturation'
    * incorporates
    *  Sum : 'Plecs\ncontroller/FB lin. vo/Sum'
    *  Constant : 'Plecs\ncontroller/FB lin. vo/Constant'
    *  Product : 'Plecs\ncontroller/FB lin. vo/Product3'
    */
   Plecs_controller_B.Saturation = 1.f -
                                   (1.f / Plecs_controller_B.Saturation1 *
                                    Plecs_controller_B.Zero_OrderHold[3]);
   if (Plecs_controller_B.Saturation < 0.0001f)
   {
      Plecs_controller_B.Saturation = 0.0001f;
   }

   /* Saturation : 'Plecs\ncontroller/FB lin io/Saturation' */
   Plecs_controller_B.Saturation_1 = Plecs_controller_B.Zero_OrderHold[5];
   if (Plecs_controller_B.Saturation_1 < 0.1f)
   {
      Plecs_controller_B.Saturation_1 = 0.1f;
   }

   /* Saturation : 'Plecs\ncontroller/Saturation'
    * incorporates
    *  Signal Switch : 'Plecs\ncontroller/Switch'
    *  Gain : 'Plecs\ncontroller/Gain'
    *  Sum : 'Plecs\ncontroller/FB lin io/Sum'
    *  Product : 'Plecs\ncontroller/FB lin io/Product1'
    *  Sum : 'Plecs\ncontroller/FB lin io/Sum1'
    *  Gain : 'Plecs\ncontroller/k_p_il'
    *  Sum : 'Plecs\ncontroller/Sum4'
    *  Product : 'Plecs\ncontroller/FB lin. vo/Product2'
    *  Sum : 'Plecs\ncontroller/Sum2'
    *  Constant : 'Plecs\ncontroller/FB lin io/one'
    */
   Plecs_controller_B.Saturation_2 =
      (Plecs_controller_B.LogicalOperator != false) ? (100.f*
                                                       Plecs_controller_B.
                                                       DiscreteIntegrator1) : (
                                                                               -((
   -(0.15f*
                                                                                    (((
                                                                                       Plecs_controller_B
                                                                                       .
   Zero_OrderHold[2] +
                                                                                       Plecs_controller_B
                                                                                       .
                                                                                       DiscreteIntegrator
   -
                                                                                       Plecs_controller_B
                                                                                       .
                                                                                       k_p_vo)
   /
                                                                                      Plecs_controller_B
                                                                                      .
                                                                                      Saturation)
   -
                                                                                     Plecs_controller_B
                                                                                     .
   Zero_OrderHold[1])) +
                                                                                  Plecs_controller_B
                                                                                  .
                                                                                  Zero_OrderHold
   [3]) /
                                                                                 Plecs_controller_B
                                                                                 .
                                                                                 Saturation_1)
   + 1.f);
   if (Plecs_controller_B.Saturation_2 > 0.75f)
   {
      Plecs_controller_B.Saturation_2 = 0.75f;
   }
   else if (Plecs_controller_B.Saturation_2 < 0.f)
   {
      Plecs_controller_B.Saturation_2 = 0.f;
   }

   /* Global output signals */
   Plecs_controller_Y.hw_outputs[0] = Plecs_controller_B.Saturation_2;
   Plecs_controller_Y.hw_outputs[1] = 0.f;
   Plecs_controller_Y.hw_outputs[2] = 1.f;
   Plecs_controller_Y.hw_outputs[3] = 1.f;
   Plecs_controller_Y.hw_outputs[4] = 1.f;
   Plecs_controller_Y.hw_outputs[5] = Plecs_controller_U.load_sw;
   Plecs_controller_Y.hw_outputs[6] = 1.f;

   if (Plecs_controller_errorStatus)
   {
      return;
   }

   /* Update for Memory : 'Plecs\ncontroller/SR Flip-flop/Memory' */
   Plecs_controller_X.Memory = Plecs_controller_B.LogicalOperator;

   /* Update for Discrete Integrator : 'Plecs\ncontroller/Discrete\nIntegrator1' */
   Plecs_controller_X.DiscreteIntegrator1_first = 0;
   Plecs_controller_X.DiscreteIntegrator1_i1_x =
      Plecs_controller_B.DiscreteIntegrator1;
   Plecs_controller_X.DiscreteIntegrator1_i2_prevU =
      Plecs_controller_B.LogicalOperator;

   /* Update for Discrete Integrator : 'Plecs\ncontroller/Discrete\nIntegrator'
    * incorporates
    *  Gain : 'Plecs\ncontroller/k_i_vo'
    *  Sum : 'Plecs\ncontroller/Sum'
    *  Signal Inport : 'Plecs\ncontroller/ref'
    */
   Plecs_controller_X.DiscreteIntegrator_first = 0;
   Plecs_controller_X.DiscreteIntegrator_i1_x =
      Plecs_controller_B.DiscreteIntegrator;
   Plecs_controller_X.DiscreteIntegrator_i2_prevU = 657.962713f*
                                                    (Plecs_controller_U.ref -
                                                     Plecs_controller_B.
                                                     Zero_OrderHold[4]);
}

void Plecs_controller_terminate(void)
{
}
