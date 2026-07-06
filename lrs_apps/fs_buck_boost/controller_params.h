
#ifndef CONTROLLER_PARAMS_H
#define CONTROLLER_PARAMS_H

#include "stdint.h"
#include "stddef.h"

typedef struct{
    float rw;
    float L;
    float Co;
    float i_abs;
    float v_norm;
    float i_norm;
    float io;
    float v_in;
    float d;
}uparams_t;

#define NX              2
#define NU              1

#define T_HORIZON       ((float)0.0036)
#define DT_GRAMPC       ((float)1e-05)
#define N_HORIZON       ((unsigned int)19)

#define MAX_GRAD_ITER   ((unsigned int)2)
#define MAX_MULT_ITER   ((unsigned int)2)

#endif /* CONTROLLER_PARAMS_H */
    
