/*
 * Implementation file for: lrs_fs_buck_boost/Plecs controller
 * Generated with         : PLECS 4.8.6
 * Generated on           : 4 Nov 2025 11:27:16
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
const char * Plecs_controller_errorStatus;
const float Plecs_controller_sampleTime = 1e-05f;
const char * const Plecs_controller_checksum =
   "551eb5c97939f78a2f4c5b7c32966e3e6709d773";
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
}

void Plecs_controller_step(void)
{
   if (Plecs_controller_errorStatus)
   {
      return;
   }

   /* Global output signals */
   Plecs_controller_Y.hw_outputs[0] = 0.f;
   Plecs_controller_Y.hw_outputs[1] = 0.f;
   Plecs_controller_Y.hw_outputs[2] = 1.f;
   Plecs_controller_Y.hw_outputs[3] = 1.f;
   Plecs_controller_Y.hw_outputs[4] = 1.f;
   Plecs_controller_Y.hw_outputs[5] = Plecs_controller_U.load_sw;
   Plecs_controller_Y.hw_outputs[6] = 0.f;

   if (Plecs_controller_errorStatus)
   {
      return;
   }
}

void Plecs_controller_terminate(void)
{
}
