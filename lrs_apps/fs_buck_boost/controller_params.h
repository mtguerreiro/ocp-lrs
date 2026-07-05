
#ifndef CONTROLLER_PARAMS_H
#define CONTROLLER_PARAMS_H

#include "stdint.h"
#include "stddef.h"

typedef struct{
    float rw;
    float io;
    float v_in;
    float d;
}uparams_t;

#define NX              2
#define NU              1

#define T_HORIZON       ((float)0.002)
#define DT_GRAMPC       ((float)1e-05)
#define N_HORIZON       ((unsigned int)11)

#define MAX_GRAD_ITER   ((unsigned int)3)
#define MAX_MULT_ITER   ((unsigned int)2)

#define RW              ((float)25)

#define Co              ((float)0.00043)
#define L               ((float)0.0001)

#define I_NORM          ((float)4)
#define V_NORM          ((float)24)

#define I_ABS           ((float)3.5)

#define TS              ((float)0.0002)



#endif /* CONTROLLER_PARAMS_H */
    
