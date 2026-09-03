/*
 * Implementation file for: lrs_fs_boost_control/Plecs controller/Single loop/Controller
 * Generated with         : PLECS 5.0.3
 * Generated on           : 3 Sep 2026 16:47:33
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
   "a8046859a71dc1d707d510f5420a457958f50003";
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

   /* Initialization for Memory : 'Controller/SR Flip-flop/Memory' */
   Plecs_controller_X.Memory = true;

   /* Initialization for Discrete Integrator : 'Controller/Discrete\nIntegrator' */
   Plecs_controller_X.DiscreteIntegrator_first = -1;
   Plecs_controller_X.DiscreteIntegrator_i1_x = 0;
   Plecs_controller_X.DiscreteIntegrator_i2_prevU = 0.f;

   /* Initialization for Discrete Integrator : 'Controller/Discrete\nIntegrator1' */
   Plecs_controller_X.DiscreteIntegrator1_first = -1;
   Plecs_controller_X.DiscreteIntegrator1_i1_x = 0;
   Plecs_controller_X.DiscreteIntegrator1_i2_prevU = 0.f;
}

void Plecs_controller_step(void)
{
   if (Plecs_controller_errorStatus)
   {
      return;
   }

   /* Zero-Order Hold : 'Controller/Zero-Order\nHold'
    * incorporates
    *  Signal Inport : 'Controller/hw_inputs'
    */
   Plecs_controller_B.Zero_OrderHold[0] = Plecs_controller_U.hw_inputs[0];
   Plecs_controller_B.Zero_OrderHold[1] = Plecs_controller_U.hw_inputs[1];
   Plecs_controller_B.Zero_OrderHold[2] = Plecs_controller_U.hw_inputs[2];
   Plecs_controller_B.Zero_OrderHold[3] = Plecs_controller_U.hw_inputs[3];
   Plecs_controller_B.Zero_OrderHold[4] = Plecs_controller_U.hw_inputs[4];
   Plecs_controller_B.Zero_OrderHold[5] = Plecs_controller_U.hw_inputs[5];

   /* Saturation : 'Controller/FB lin io/Saturation' */
   Plecs_controller_B.Saturation = Plecs_controller_B.Zero_OrderHold[5];
   if (Plecs_controller_B.Saturation < 0.1f)
   {
      Plecs_controller_B.Saturation = 0.1f;
   }

   /* Gain : 'Controller/k_p_vo' */
   Plecs_controller_B.k_p_vo = 1.14666667f*
                               Plecs_controller_B.Zero_OrderHold[4];

   /* Saturation : 'Controller/FB lin. vo/Saturation1'
    * incorporates
    *  Signal Inport : 'Controller/ref'
    */
   Plecs_controller_B.Saturation1 = Plecs_controller_U.ref;
   if (Plecs_controller_B.Saturation1 < 0.0001f)
   {
      Plecs_controller_B.Saturation1 = 0.0001f;
   }

   /* Saturation : 'Controller/FB lin. vo/Saturation'
    * incorporates
    *  Sum : 'Controller/FB lin. vo/Sum'
    *  Constant : 'Controller/FB lin. vo/Constant'
    *  Product : 'Controller/FB lin. vo/Product3'
    */
   Plecs_controller_B.Saturation_1 = 1.f -
                                     (1.f / Plecs_controller_B.Saturation1 *
                                      Plecs_controller_B.Zero_OrderHold[3]);
   if (Plecs_controller_B.Saturation_1 < 0.0001f)
   {
      Plecs_controller_B.Saturation_1 = 0.0001f;
   }

   /* Memory : 'Controller/SR Flip-flop/Memory' */
   Plecs_controller_B.Memory = Plecs_controller_X.Memory;

   /* Comparator : 'Controller/Comparator' */
   if (Plecs_controller_B.Zero_OrderHold[5] > Plecs_controller_U.ref)
      Plecs_controller_B.Comparator = 1;
   else if (Plecs_controller_B.Zero_OrderHold[5] < Plecs_controller_U.ref)
      Plecs_controller_B.Comparator = 0;
   /* Logical Operator : 'Controller/SR Flip-flop/Logical\nOperator'
    * incorporates
    *  Logical Operator : 'Controller/SR Flip-flop/Logical\nOperator1'
    *  Logical Operator : 'Controller/SR Flip-flop/Logical\nOperator2'
    *  Subsystem : 'Controller'
    */
   Plecs_controller_B.LogicalOperator = (!Plecs_controller_B.Comparator) &&
                                        (Plecs_controller_UNCONNECTED ||
                                         Plecs_controller_B.Memory);

   /* Discrete Integrator : 'Controller/Discrete\nIntegrator' */
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

   /* Discrete Integrator : 'Controller/Discrete\nIntegrator1'
    * incorporates
    *  Logical Operator : 'Controller/SR Flip-flop/Logical\nOperator4'
    *  Logical Operator : 'Controller/SR Flip-flop/Logical\nOperator3'
    *  Subsystem : 'Controller'
    */
   if (Plecs_controller_X.DiscreteIntegrator1_first < 0 ||
       (!(Plecs_controller_B.LogicalOperator ||
          (Plecs_controller_UNCONNECTED &&
           Plecs_controller_B.Comparator)) != 0))
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

   /* Saturation : 'Controller/Saturation'
    * incorporates
    *  Signal Switch : 'Controller/Switch'
    *  Gain : 'Controller/Gain'
    *  Sum : 'Controller/FB lin io/Sum'
    *  Product : 'Controller/FB lin io/Product1'
    *  Sum : 'Controller/FB lin io/Sum1'
    *  Gain : 'Controller/k_p_il'
    *  Sum : 'Controller/Sum4'
    *  Product : 'Controller/FB lin. vo/Product2'
    *  Sum : 'Controller/Sum2'
    *  Constant : 'Controller/FB lin io/one'
    */
   Plecs_controller_B.Saturation_2 =
      (Plecs_controller_B.LogicalOperator != false) ? (100.f*
                                                       Plecs_controller_B.
                                                       DiscreteIntegrator1) : (
                                                                               -((
   -(0.666666667f*
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
                                                                                      Saturation_1)
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
                                                                                 Saturation)
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

   /* Update for Memory : 'Controller/SR Flip-flop/Memory' */
   Plecs_controller_X.Memory = Plecs_controller_B.LogicalOperator;

   /* Update for Discrete Integrator : 'Controller/Discrete\nIntegrator'
    * incorporates
    *  Gain : 'Controller/k_i_vo'
    *  Sum : 'Controller/Sum'
    *  Signal Inport : 'Controller/ref'
    */
   Plecs_controller_X.DiscreteIntegrator_first = 0;
   Plecs_controller_X.DiscreteIntegrator_i1_x =
      Plecs_controller_B.DiscreteIntegrator;
   Plecs_controller_X.DiscreteIntegrator_i2_prevU = 1257.43985f*
                                                    (Plecs_controller_U.ref -
                                                     Plecs_controller_B.
                                                     Zero_OrderHold[4]);

   /* Update for Discrete Integrator : 'Controller/Discrete\nIntegrator1' */
   Plecs_controller_X.DiscreteIntegrator1_first = 0;
   Plecs_controller_X.DiscreteIntegrator1_i1_x =
      Plecs_controller_B.DiscreteIntegrator1;
   Plecs_controller_X.DiscreteIntegrator1_i2_prevU =
      Plecs_controller_B.LogicalOperator;
}

void Plecs_controller_terminate(void)
{
}
