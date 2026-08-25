/*
 * Implementation file for: lrs_fs_boost_control/Controller/Cascaded/Plecs controller_inner_loop
 * Generated with         : PLECS 4.8.10
 * Generated on           : 24 Aug 2026 15:10:03
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
const char * Plecs_controller_inner_loop_errorStatus;
const float Plecs_controller_inner_loop_sampleTime = 1e-05f;
const char * const Plecs_controller_inner_loop_checksum =
   "473f6dc1151cfa8f3d6d127571bfda9c0e318644";
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
}

void Plecs_controller_inner_loop_step(void)
{
   if (Plecs_controller_inner_loop_errorStatus)
   {
      return;
   }

   /* Saturation : 'Plecs\ncontroller_inner_loop/FB lin. vo/Saturation1'
    * incorporates
    *  Zero-Order Hold : 'Plecs\ncontroller_inner_loop/Zero-Order\nHold'
    *  Signal Inport : 'Plecs\ncontroller_inner_loop/hw_inputs'
    */
   Plecs_controller_inner_loop_B.Saturation1 =
      Plecs_controller_inner_loop_U.hw_inputs[4];
   if (Plecs_controller_inner_loop_B.Saturation1 < 1e-05f)
   {
      Plecs_controller_inner_loop_B.Saturation1 = 1e-05f;
   }

   /* Saturation : 'Plecs\ncontroller_inner_loop/Saturation'
    * incorporates
    *  Sum : 'Plecs\ncontroller_inner_loop/FB lin. vo/Sum2'
    *  Constant : 'Plecs\ncontroller_inner_loop/FB lin. vo/Constant'
    *  Product : 'Plecs\ncontroller_inner_loop/FB lin. vo/Product3'
    *  Sum : 'Plecs\ncontroller_inner_loop/FB lin. vo/Sum1'
    *  Zero-Order Hold : 'Plecs\ncontroller_inner_loop/Zero-Order\nHold'
    *  Signal Inport : 'Plecs\ncontroller_inner_loop/hw_inputs'
    *  Gain : 'Plecs\ncontroller_inner_loop/L'
    *  Gain : 'Plecs\ncontroller_inner_loop/k_p_c'
    *  Sum : 'Plecs\ncontroller_inner_loop/Sum'
    *  Signal Inport : 'Plecs\ncontroller_inner_loop/internal_ref'
    */
   Plecs_controller_inner_loop_B.Saturation = 1.f -
                                              (1.f /
                                               Plecs_controller_inner_loop_B.
                                               Saturation1 *
                                               ((Plecs_controller_inner_loop_U
                                                 .hw_inputs[3]) -
                                                (0.0001f*
      (8000.f*
      (Plecs_controller_inner_loop_U.internal_ref -
      (Plecs_controller_inner_loop_U.hw_inputs[1]))))));
   if (Plecs_controller_inner_loop_B.Saturation > 0.99f)
   {
      Plecs_controller_inner_loop_B.Saturation = 0.99f;
   }
   else if (Plecs_controller_inner_loop_B.Saturation < 0.f)
   {
      Plecs_controller_inner_loop_B.Saturation = 0.f;
   }

   /* Global output signals */
   Plecs_controller_inner_loop_Y.hw_outputs[0] =
      Plecs_controller_inner_loop_B.Saturation;
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
}

void Plecs_controller_inner_loop_terminate(void)
{
}
