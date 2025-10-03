//#############################################################################
//
// FILE:   empty_driverlib_main_cpu2.c
//
// TITLE:  Empty Project
//
// CPU2 Empty Project Example
//
// This example is an empty project setup for Driverlib development for CPU2.
//
//#############################################################################
//
// 
// $Copyright:
// Copyright (C) 2013-2025 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"

#include "c2000Config.h"

#include "ipc/ipcServer.h"
#include "ipcServerC2000.h"

#include "ocp/ocpIf.h"

#include "app/app.h"
#include "controller/controllerIf.h"

//=============================================================================
/*------------------------------- Prototypes --------------------------------*/
//=============================================================================
static void mainInit(void);
static void mainInitC2000(void);
static void mainInitIpc(void);
//=============================================================================

//=============================================================================
/*----------------------------------- Main ----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void main(void)
{
    mainInit();

    while(1);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void mainInit(void){

    mainInitC2000();

    mainInitIpc();

    ocpIfInitialize();
    controllerIfInit();
    appInit();
}
//-----------------------------------------------------------------------------
static void mainInitC2000(void){

    /*
     * Initialize device clock and peripherals. We don't initialize GPIOs
     * because this will be done by CPU1.
     */
    Device_init();

    /* Initialize PIE and clear PIE registers. Disables CPU interrupts */
    Interrupt_initModule();

    /*
     * Initialize the PIE vector table with pointers to the shell Interrupt
     * Service Routines (ISR).
     */
    Interrupt_initVectorTable();

    /* Waits until CPU1 has initialized */
    while( !(HWREG(IPC_BASE + IPC_O_STS) & (1UL << C2000_CONFIG_CPU1_INIT)) );

    /* Acks the IPC flag */
    HWREG(IPC_BASE + IPC_O_ACK) = 1UL << C2000_CONFIG_CPU1_INIT;
    HWREG(IPC_BASE + IPC_O_CLR) = 1UL << C2000_CONFIG_CPU1_INIT;

    /* Enable Global Interrupt (INTM) and realtime interrupt (DBGM) */
    EINT;
    ERTM;

    /* Signals CPU1 that we have initialized CPU2 */
    HWREG(IPC_BASE + IPC_O_SET) = 1UL << C2000_CONFIG_CPU2_INIT;
}
//-----------------------------------------------------------------------------
static void mainInitIpc(void){

    ipcServerC2000Initialize(0);
    ipcServerInitialize(
        ocpIf, ipcServerC2000IrqSend,
        C2000_CONFIG_MEM_CPU1_TO_CPU2_ADR, C2000_CONFIG_MEM_CPU1_TO_CPU2_SIZE,
        C2000_CONFIG_MEM_CPU2_TO_CPU1_ADR, C2000_CONFIG_MEM_CPU2_TO_CPU1_SIZE
    );
}
//-----------------------------------------------------------------------------
//=============================================================================
