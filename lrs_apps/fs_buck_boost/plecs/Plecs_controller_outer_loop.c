/*
 * Implementation file for: lrs_fs_boost_control/Controller/cascade_block_control/Plecs controller_outer_loop
 * Generated with         : PLECS 4.8.10
 * Generated on           : 24 Jul 2026 15:04:36
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
const float Plecs_controller_outer_loop_sampleTime = 0.0001f;
const char * const Plecs_controller_outer_loop_checksum =
   "dd3c79bc6e297416856c0379964e15875b12fba9";
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

   /* Initialization for Discrete Integrator : 'Plecs\ncontroller_outer_loop/Discrete\nIntegrator' */
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

   /* Sum : 'Plecs\ncontroller_outer_loop/Sum'
    * incorporates
    *  Signal Inport : 'Plecs\ncontroller_outer_loop/ref'
    *  Zero-Order Hold : 'Plecs\ncontroller_outer_loop/Zero-Order\nHold'
    *  Signal Inport : 'Plecs\ncontroller_outer_loop/hw_inputs'
    */
   Plecs_controller_outer_loop_B.Sum = Plecs_controller_outer_loop_U.ref -
                                       (Plecs_controller_outer_loop_U.
                                        hw_inputs[4]);

   /* Discrete Integrator : 'Plecs\ncontroller_outer_loop/Discrete\nIntegrator' */
   if (Plecs_controller_outer_loop_X.DiscreteIntegrator_first < 0)
   {
      Plecs_controller_outer_loop_B.DiscreteIntegrator = 0.f;
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
         (Plecs_controller_outer_loop_X.DiscreteIntegrator_i2_prevU +
          Plecs_controller_outer_loop_B.Sum);
   }

   /* Global output signals */
   Plecs_controller_outer_loop_Y.internal_ref =
      (Plecs_controller_outer_loop_U.hw_inputs[2]) +
      (200.f*
       Plecs_controller_outer_loop_B.DiscreteIntegrator) -
      (0.1f*Plecs_controller_outer_loop_B.Sum);

   if (Plecs_controller_outer_loop_errorStatus)
   {
      return;
   }

   /* Update for Discrete Integrator : 'Plecs\ncontroller_outer_loop/Discrete\nIntegrator' */
   Plecs_controller_outer_loop_X.DiscreteIntegrator_first = 0;
   Plecs_controller_outer_loop_X.DiscreteIntegrator_i1_x =
      Plecs_controller_outer_loop_B.DiscreteIntegrator;
   Plecs_controller_outer_loop_X.DiscreteIntegrator_i2_prevU =
      Plecs_controller_outer_loop_B.Sum;
}

void Plecs_controller_outer_loop_terminate(void)
{
}
