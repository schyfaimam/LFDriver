/*===========================================================================*/
/*                                                                           */
/* Project     = F1L  Software                                               */
/* Module      = LFDriver.h                                                  */
/* Version     = $Revision: 1131 $                                           */
/*                                                                           */
/*                                                                           */
/*============================================================================ */
/* */
/* Header file for the LFDriver source code */
/* */
/*============================================================================ */
#include "Platform_Types.h"

#ifndef _LFDriver_H
#define _LFDriver_H

/*============================================================================ */
/* Defines */
/*============================================================================ */
#define PWMPERIOD 80
#define PWMDUTY 40

typedef enum 
{
    LFDrv_IDLE,
    LFDrv_BUSY,
}LFDrvStatusReturnType;

typedef enum
{
    LFDrvIsIdle,
    LFDrvIsBusy,
}LFDrvStatusWord;

/*============================================================================ */
/* Function defines */
/*============================================================================ */
void LFDrv_Init(void);
static void LFDrv_ClkInit(void);
static void LFDrv_IOInit(void);
static void LFDrv_PwmConfig_Init(void);
void LFDrv_Deinit(void);
static void LFDrv_ClkDeinit(void);
static void LFDrv_IODeinit(void);
static void LFDrv_PwmConfig_Deinit(void);
boolean LFDrv_SetDataBuffer(uint8 *databuff,uint8 Siz);
static void LFDrv_DmaConfig(void);
void LFDrv_ClearDataBuffer(void);
void LFDrv_RestCmd(void);
void LFDrv_AsyncTrsmStart(void);
void LFDrv_AsyncTrsmStop(void);
LFDrvStatusReturnType LFDrv_GetStatus(void);
void LFDrv_DMACmpInterrupt(void);

#endif
