/*
 * Implementation file for: lrs_buck_control/Plecs controller
 * Generated with         : PLECS 4.8.10
 * Generated on           : 10 Nov 2025 15:14:15
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
   "53a35e094d593f87255d3e347336228331b9a754";
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

   /* Discrete Integrator : 'Plecs\ncontroller/Discrete\nIntegrator'
    * incorporates
    *  Sum : 'Plecs\ncontroller/Sum'
    *  Signal Inport : 'Plecs\ncontroller/ref'
    */
   if (Plecs_controller_X.DiscreteIntegrator_first < 0)
   {
      Plecs_controller_B.DiscreteIntegrator = 0.f;
   }
   else if (Plecs_controller_X.DiscreteIntegrator_first)
   {
      Plecs_controller_B.DiscreteIntegrator =
         Plecs_controller_X.DiscreteIntegrator_i1_x;
   }
   else
   {
      Plecs_controller_B.DiscreteIntegrator =
         Plecs_controller_X.DiscreteIntegrator_i1_x + 5e-06f*
         (Plecs_controller_X.DiscreteIntegrator_i2_prevU +
          Plecs_controller_U.ref - Plecs_controller_B.Zero_OrderHold[4]);
   }

   /* Saturation : 'Plecs\ncontroller/Saturation'
    * incorporates
    *  Sum : 'Plecs\ncontroller/Sum2'
    *  Sum : 'Plecs\ncontroller/Sum1'
    *  Gain : 'Plecs\ncontroller/k_ev'
    *  Gain : 'Plecs\ncontroller/kv'
    *  Gain : 'Plecs\ncontroller/ki'
    */
   Plecs_controller_B.Saturation =
      (-(-138.163513f*
         Plecs_controller_B.DiscreteIntegrator) -
       (0.0130563067f*
     Plecs_controller_B.Zero_OrderHold[4])) -
      (0.0406004464f*Plecs_controller_B.Zero_OrderHold[0]);
   if (Plecs_controller_B.Saturation > 1.f)
   {
      Plecs_controller_B.Saturation = 1.f;
   }
   else if (Plecs_controller_B.Saturation < 0.f)
   {
      Plecs_controller_B.Saturation = 0.f;
   }

   /* Global output signals */
   Plecs_controller_Y.hw_outputs[0] = Plecs_controller_B.Saturation;
   Plecs_controller_Y.hw_outputs[1] = 1.f;
   Plecs_controller_Y.hw_outputs[2] = Plecs_controller_U.output_relay;

   if (Plecs_controller_errorStatus)
   {
      return;
   }

   /* Update for Discrete Integrator : 'Plecs\ncontroller/Discrete\nIntegrator'
    * incorporates
    *  Sum : 'Plecs\ncontroller/Sum'
    *  Signal Inport : 'Plecs\ncontroller/ref'
    */
   Plecs_controller_X.DiscreteIntegrator_first = 0;
   Plecs_controller_X.DiscreteIntegrator_i1_x =
      Plecs_controller_B.DiscreteIntegrator;
   Plecs_controller_X.DiscreteIntegrator_i2_prevU = Plecs_controller_U.ref -
                                                    Plecs_controller_B.
                                                    Zero_OrderHold[4];
}

void Plecs_controller_terminate(void)
{
}
