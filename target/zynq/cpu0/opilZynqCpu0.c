/*
 * @file opilZynqCpu0.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "opilZynqCpu0.h"

#include "ocp/ocpIf.h"

#include "ipc/ipcClient.h"

#include "stypes.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define OPIL_ZYNQ_CPU0_REQ_TO       100
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static stypesControl_t xtControl;
static stypesControllerData_t xtControllerData;

static uint8_t txbuf[256];
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void opilZynqCpu0Initialize(void){

}
//-----------------------------------------------------------------------------
int32_t opilZynqCpu0UpdateMeasurements(void *meas, int32_t size){

    int32_t status;
    int32_t *p;
    uint32_t k;

    uint8_t *dst, *src;

    k = size;
    p = (int32_t *)txbuf;
    *p = OCP_IF_CMD_OPIL_UPDATE_MEASUREMENTS;

    dst = &txbuf[4];
    src = (uint8_t *)meas;
    for(k = 0; k < (uint32_t) size; k++) *dst++ = *src++;

    status = ipcClientRequest( (void *)&txbuf, size + 4, 0, 0, OPIL_ZYNQ_CPU0_REQ_TO );
    if( status < 0 ) return status;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t opilZynqCpu0UpdateSimData(void *simData, int32_t size){

    int32_t status;
    int32_t *p;
    uint32_t k;

    uint8_t *dst, *src;

    k = size;
    p = (int32_t *)txbuf;
    *p = OCP_IF_CMD_OPIL_UPDATE_SIM_DATA;

    dst = &txbuf[4];
    src = (uint8_t *)simData;
    for(k = 0; k < (uint32_t) size; k++) *dst++ = *src++;

    status = ipcClientRequest( (void *)&txbuf, size + 4, 0, 0, OPIL_ZYNQ_CPU0_REQ_TO );
    if( status < 0 ) return status;

    return 0;
}
//-----------------------------------------------------------------------------
void opilZynqCpu0RunControl(void){

    int32_t *p;

    p = (int32_t *)txbuf;
    *p = OCP_IF_CMD_OPIL_RUN_CONTROL;

    ipcClientRequest( (void *)&txbuf, 4, 0, 0, OPIL_ZYNQ_CPU0_REQ_TO );
}
//-----------------------------------------------------------------------------
int32_t opilZynqCpu0GetControl(void **control){

    void *vp;
    int32_t size;
    int32_t *p;

    p = (int32_t *)txbuf;
    *p = OCP_IF_CMD_OPIL_GET_CONTROL;

    vp = (void *)&xtControl;
    size = ipcClientRequest( (void *)&txbuf, 4, (void **)&vp, sizeof(stypesControl_t), OPIL_ZYNQ_CPU0_REQ_TO );

    *control = &xtControl;

    return size;
}
//-----------------------------------------------------------------------------
int32_t opilZynqCpu0GetControllerData(void **controllerData){

    int32_t size;
    int32_t *p;

    p = (int32_t *)txbuf;
    *p = OCP_IF_CMD_OPIL_GET_CONTROLLER_DATA;

    size = ipcClientRequest( (void *)&txbuf, 4, (void **)&xtControllerData, sizeof(stypesControllerData_t), OPIL_ZYNQ_CPU0_REQ_TO );

    *controllerData = &xtControllerData;

    return size;
}
//-----------------------------------------------------------------------------
//=============================================================================
