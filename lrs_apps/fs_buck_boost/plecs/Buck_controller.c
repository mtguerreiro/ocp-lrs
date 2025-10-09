/*
 * Implementation file for: lrs_fs_buck_boost/Buck controller
 * Generated with         : PLECS 4.8.10
 * Generated on           : 8 Oct 2025 19:11:08
 */
#include "Buck_controller.h"
#ifndef PLECS_HEADER_Buck_controller_h_
#error The wrong header file "Buck_controller.h" was included. Please check
#error your include path to see whether this file name conflicts with the
#error name of another header file.
#endif /* PLECS_HEADER_Buck_controller_h_ */
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
#define PLECSRunTimeError(msg) Buck_controller_errorStatus = msg
#define Buck_controller_UNCONNECTED 0
static uint32_t Buck_controller_tickLo;
static int32_t Buck_controller_tickHi;
Buck_controller_ExternalInputs Buck_controller_U;
Buck_controller_ExternalOutputs Buck_controller_Y;
Buck_controller_BlockOutputs Buck_controller_B;
Buck_controller_ModelStates Buck_controller_X _ALIGN;
const char * Buck_controller_errorStatus;
const float Buck_controller_sampleTime = 1e-05f;
const char * const Buck_controller_checksum =
   "0b2840e95a8487a98ef743feb1690695f9111177";
void Buck_controller_initialize(float time)
{
   float remainder;
   Buck_controller_errorStatus = NULL;
   Buck_controller_tickHi =
      floor(time/(4294967296.0*Buck_controller_sampleTime));
   remainder = time - Buck_controller_tickHi*4294967296.0*
               Buck_controller_sampleTime;
   Buck_controller_tickLo = floor(remainder/Buck_controller_sampleTime + .5);
   remainder -= Buck_controller_tickLo*Buck_controller_sampleTime;
   if (fabsf(remainder) > 1e-6*fabsf(time))
   {
      Buck_controller_errorStatus =
         "Start time must be an integer multiple of the base sample time.";
   }
   memset(&Buck_controller_X, 0, sizeof(Buck_controller_X));

   /* Initialization for Discrete Integrator : 'Buck\ncontroller/Discrete\nIntegrator' */
   Buck_controller_X.DiscreteIntegrator_first = -1;
   Buck_controller_X.DiscreteIntegrator_i1_x = 0;
   Buck_controller_X.DiscreteIntegrator_i2_prevU = 0.f;
}

void Buck_controller_step(void)
{
   if (Buck_controller_errorStatus)
   {
      return;
   }

   /* Zero-Order Hold : 'Buck\ncontroller/Zero-Order\nHold'
    * incorporates
    *  Signal Inport : 'Buck\ncontroller/hw_inputs'
    */
   Buck_controller_B.Zero_OrderHold[0] = Buck_controller_U.hw_inputs[0];
   Buck_controller_B.Zero_OrderHold[1] = Buck_controller_U.hw_inputs[1];
   Buck_controller_B.Zero_OrderHold[2] = Buck_controller_U.hw_inputs[2];
   Buck_controller_B.Zero_OrderHold[3] = Buck_controller_U.hw_inputs[3];
   Buck_controller_B.Zero_OrderHold[4] = Buck_controller_U.hw_inputs[4];
   Buck_controller_B.Zero_OrderHold[5] = Buck_controller_U.hw_inputs[5];

   /* Discrete Integrator : 'Buck\ncontroller/Discrete\nIntegrator'
    * incorporates
    *  Sum : 'Buck\ncontroller/Sum'
    *  Signal Inport : 'Buck\ncontroller/ref'
    */
   if (Buck_controller_X.DiscreteIntegrator_first < 0)
   {
      Buck_controller_B.DiscreteIntegrator = 0.f;
   }
   else if (Buck_controller_X.DiscreteIntegrator_first)
   {
      Buck_controller_B.DiscreteIntegrator =
         Buck_controller_X.DiscreteIntegrator_i1_x;
   }
   else
   {
      Buck_controller_B.DiscreteIntegrator =
         Buck_controller_X.DiscreteIntegrator_i1_x + 5e-06f*
         (Buck_controller_X.DiscreteIntegrator_i2_prevU +
          Buck_controller_U.ref -
          Buck_controller_B.Zero_OrderHold[4]);
   }

   /* Saturation : 'Buck\ncontroller/Saturation'
    * incorporates
    *  Sum : 'Buck\ncontroller/Sum2'
    *  Sum : 'Buck\ncontroller/Sum1'
    *  Gain : 'Buck\ncontroller/k_ev'
    *  Gain : 'Buck\ncontroller/kv'
    *  Gain : 'Buck\ncontroller/ki'
    */
   Buck_controller_B.Saturation =
      (-(-33.595991f*
         Buck_controller_B.DiscreteIntegrator) -
       (-0.0268410338f*
     Buck_controller_B.Zero_OrderHold[4])) -
      (0.01275f*Buck_controller_B.Zero_OrderHold[1]);
   if (Buck_controller_B.Saturation > 1.f)
   {
      Buck_controller_B.Saturation = 1.f;
   }
   else if (Buck_controller_B.Saturation < 0.f)
   {
      Buck_controller_B.Saturation = 0.f;
   }

   /* Global output signals */
   Buck_controller_Y.hw_outputs[0] = Buck_controller_B.Saturation;
   Buck_controller_Y.hw_outputs[1] = 0.f;
   Buck_controller_Y.hw_outputs[2] = 1.f;
   Buck_controller_Y.hw_outputs[3] = 1.f;
   Buck_controller_Y.hw_outputs[4] = 1.f;
   Buck_controller_Y.hw_outputs[5] = Buck_controller_U.load_sw;
   Buck_controller_Y.hw_outputs[6] = 0.f;

   if (Buck_controller_errorStatus)
   {
      return;
   }

   /* Update for Discrete Integrator : 'Buck\ncontroller/Discrete\nIntegrator'
    * incorporates
    *  Sum : 'Buck\ncontroller/Sum'
    *  Signal Inport : 'Buck\ncontroller/ref'
    */
   Buck_controller_X.DiscreteIntegrator_first = 0;
   Buck_controller_X.DiscreteIntegrator_i1_x =
      Buck_controller_B.DiscreteIntegrator;
   Buck_controller_X.DiscreteIntegrator_i2_prevU = Buck_controller_U.ref -
                                                   Buck_controller_B.
                                                   Zero_OrderHold[4];
}

void Buck_controller_terminate(void)
{
}
