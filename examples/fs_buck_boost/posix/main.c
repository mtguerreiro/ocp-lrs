//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "ocp/ocpIf.h"

#include "controller/controllerIf.h"

#include "fsbuckboost.h"

#include "target/posix/threads/ocpIfThread.h"
#include "target/posix/threads/ocpOpilThread.h"
//=============================================================================

//=============================================================================
/*---------------------------------- Main -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int main(void){

    pthread_t ocpIfThreadHandle, ocpOpilThreadHandle;

    ocpIfInitialize();
    controllerIfInit();

    fsbuckboostInit((void *)0);

    pthread_create( &ocpIfThreadHandle, NULL, ocpIfThread, NULL );
    pthread_create( &ocpOpilThreadHandle, NULL, ocpOpilThread, NULL );

    pthread_join( ocpIfThreadHandle, NULL );

    exit(0);
}
//-----------------------------------------------------------------------------
//=============================================================================
