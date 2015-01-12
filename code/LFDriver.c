/*===========================================================================*/
/*                                                                           */
/* Project     = F1L Software                                                */
/* Module      = LFDriver.c                                                  */
/* Version     = $Revision: 1.0 $                                            */
/*                                                                           */
/*                                                                           */
/*============================================================================ */
/*                                                                             */
/* Source code for the LFDriver                                                */
/*                                                                             */
/*============================================================================ */

/*============================================================================ */
/* Includes */
/*============================================================================ */
#include "dr7f701035_0.h"
#include "LFDriver.h"
#include "Platform_Types.h"

/*============================================================================ */
/* Parameters and Variables*/
/*============================================================================ */
static uint8 PwmDataBuffer[22],PwmDataBufferIdx,PwmDataBitIdx;
static uint16 PwmDutyBuffer[180],PwmDutyBufferIdx;
static uint16 PwmPeriodBuffer[180],PwmPeriodBufferIdx;
static uint16 PwmTolBuffer[180],PwmTolBufferIdx;
static uint16 PwmRdtBuffer[2]; 
static uint8 PwmTrsmState;

/*============================================================================ */
/* Functions */
/*============================================================================ */
/******************************************************************************
** Function:    LFDrv_Init
** Description: Initializes the LF Driver
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
void LFDrv_Init(void)
{
    LFDrv_ClkInit();  /*set TAUD0 clock to PLL(10MHz)*/
    LFDrv_IOInit();   /*Set P10_2 as TAUD0O5*/
    LFDrv_PwmConfig_Init(); /*Config TAUD0 Ch12,CH13 as PWM Output function*/

    PwmTrsmState=LFDrvIsIdle;   /*Set State to Idle*/ 
}


/******************************************************************************
** Function:    LFDrv_ClkInit
** Description: Initializes the Clock for PWM timer
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
static void LFDrv_ClkInit(void)
{
    /* Set TAUD0 Clock to PLL(10MHz)*/
    do
    {
        PROTCMD1 = 0xA5;
        CKSC_IPERI1S_CTL = 0x02;
        CKSC_IPERI1S_CTL = ~0x02;
        CKSC_IPERI1S_CTL = 0x02;
    }while(PROTS1==0x01);
    while(CKSC_IPERI1S_ACT!=0x02){}/*wait selected clock act*/   
}


/******************************************************************************
** Function:    LFDrv_IOInit
** Description: Initializes the PIN for PWM output
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
static void LFDrv_IOInit(void)
{
    /*Setting P10_2 as TAUD0O5 function*/
    PMC10 |= 0x0004;
    PIPC10 &= 0xFFFB;
    PM10 &= 0xFFFB;
    PFCAE10 &= 0xFFFB;
    PFCE10 &= 0xFFFB;
    PFC10 &= 0xFFFB;
}


/******************************************************************************
** Function:    LFDrv_PwmConfig_Init
** Description: Initializes the TAU configuration for PWM
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
static void LFDrv_PwmConfig_Init(void)
{
    /*Select CK0 for Master and Slave count*/
    TAUD0TPS &= 0xFFF0;   /*Init CK0=PCLK*/

    /*Register setting for Master Channel*/
    TAUD0CMOR4 = 0x0801;  /*CK0,Master channel,Interval timer mode */
    TAUD0CMUR4 = 0x0000;

    /*Simultaneous rewrite setting*/
    TAUD0RDE |= 0x0010;   /*Enable*/
    TAUD0RDS &= 0xFFEF;
    TAUD0RDM &= 0xFFEF;
    TAUD0RDC &= 0xFFEF;

    /*Register setting for Slave Channel*/
    TAUD0CMOR5 = 0x0409;  /*CK0,Slave channel,One-count mode*/
    TAUD0CMUR5 = 0x0000;

    /*Slave Channel output mode*/
    TAUD0TOE &= 0xFFDF;
    TAUD0TO &= 0xFFDF;   /*Set TAUD0O5 Low*/
    TAUD0TOE |= 0x0020;  /*Enables independent channel output mode*/
    TAUD0TOM |= 0x0020;  /*Synchronous channel operation*/
    TAUD0TOC &= 0xFFDF;  /*Operating mode 1*/
    TAUD0TOL &= 0xFFDF;  /*Positive logic*/

    /*Simultaneous rewrite setting*/
    TAUD0RDE |= 0x0020;  /*Enable*/
    TAUD0RDS &= 0xFFDF;
    TAUD0RDM &= 0xFFDF;
    TAUD0RDC &= 0xFFDF;    
}


/******************************************************************************
** Function:    LFDrv_Deinit
** Description: Deinitializes the LF Driver
** Parameter:   
**
**              
**              
** Return:      None
******************************************************************************/
void LFDrv_Deinit(void)
{
    LFDrv_ClkDeinit();
    LFDrv_IODeinit();
    LFDrv_PwmConfig_Deinit();
}
/******************************************************************************
** Function:    LFDrv_ClkDeinit
** Description: Deinitializes the Clock for PWM timer
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
static void LFDrv_ClkDeinit(void)
{
    /* Set TAUD0 Clock to MainOSC*/
    do
    {
        PROTCMD1 = 0xA5;
        CKSC_IPERI1S_CTL = 0x01;
        CKSC_IPERI1S_CTL = ~0x01;
        CKSC_IPERI1S_CTL = 0x01;
    }while(PROTS1==0x01);
    while(CKSC_IPERI1S_ACT!=0x01){}  
}


/******************************************************************************
** Function:    LFDrv_IODeinit
** Description: Deinitializes the port for PWM output
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
static void LFDrv_IODeinit(void)
{
    PMC10 &= 0xFFFB;
    PIPC10 &= 0xFFFB;
    PM10 &= 0xFFFB;
    PFCAE10 &= 0xFFFB;
    PFCE10 &= 0xFFFB;
    PFC10 &= 0xFFFB;  
}


/******************************************************************************
** Function:    LFDrv_PwmConfig_Deinit
** Description: Deinitializes the TAU configuration for PWM
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
static void LFDrv_PwmConfig_Deinit(void)
{
    /*release CK0*/
    TAUD0TPS |= 0x000F;   /*POR CK0=PCLK/15*/

    /*Register setting for Master Channel*/
    TAUD0CMOR4 = 0x0000;  /*POR Value */
    TAUD0CMUR4 = 0x0000;

    /*Simultaneous rewrite setting*/
    TAUD0RDE &= 0xFFEF;   /*Disable*/

    /*Register setting for Slave Channel*/
    TAUD0CMOR5 = 0x0000;  /*POR Value*/
    TAUD0CMUR5 = 0x0000;

    /*Slave Channel output mode*/
    TAUD0TO &= 0xFFDF;   
    TAUD0TOE &= 0xFFDF;  
    TAUD0TOM &= 0xFFDF;  
    TAUD0TOC &= 0xFFDF;  
    TAUD0TOL &= 0xFFDF;  

    /*Simultaneous rewrite setting*/
    TAUD0RDE &= 0xFFDF;  /*Disable*/  
}


/******************************************************************************
** Function:    LFDrv_DmaConfig
** Description: Config the DMA for transfer PWM period and duty
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
static void LFDrv_DmaConfig(void)
{
    DRQCLR |= 0x000F;    /*Clear transfer request flag*/
    
    /*DMA0 -- TAUD0RDT*/
    DTS0 &= 0xFC;               /*Disable DMA0*/
    DSA0 = (u32_T)PwmRdtBuffer; /*Source address setting*/
    DDA0 = 0xFFE20044;          /*Destination address setting*/
    DTCT0 = 0x20A0;		/*16bit transfer,source&destination address fixed*/
    DTC0 = PwmTolBufferIdx+1;	/*transfer times*/
    DTFR0 = 0x8001;             /*enable transfer trigger factor*/
    DTS0 |= 0x01;               /*enable DMA0*/
  
    /*DMA1 -- TAUD0CDR4*/
    DTS1 &= 0xFC;                  /*Disable DMA1*/
    DSA1 = (u32_T)PwmPeriodBuffer; /*Source address setting*/
    DDA1 = 0xFFE20010;             /*Destination address setting*/
    DTCT1 = 0x2020;                /*16bit transfer,source address increment,destination address fixed*/
    DTC1 = PwmTolBufferIdx+1;      /*transfer times*/
    DTFR1 = 0x8001;                /*enable transfer trigger factor*/
    DTS1 |= 0x01;                  /*enable DMA1*/
  
    /*DMA2 -- TAUD0CDR5*/
    DTS2 &= 0xFC;                  /*Disable DMA2*/
    DSA2 = (u32_T)PwmDutyBuffer;   /*Source address setting*/
    DDA2 = 0xFFE20014;             /*Destination address setting*/
    DTCT2 = 0x2020;                /*16bit transfer,source address increment,destination address fixed*/
    DTC2 = PwmTolBufferIdx+1;      /*transfer times*/
    DTFR2 = 0x8001;                /*enable transfer trigger factor*/
    DTS2 |= 0x01;                  /*enable DMA2*/

    /*DMA3 -- TAUD0TOL*/
    DTS3 &= 0xFC;                  /*Disable DMA3*/
    DSA3 = (u32_T)PwmTolBuffer;    /*Source address setting*/
    DDA3 = 0xFFE20040;             /*Destination address setting*/
    DTCT3 = 0x2020;                /*16bit transfer,source address increment,destination address fixed*/
    DTC3 = PwmTolBufferIdx+1;      /*transfer times*/
    DTFR3 = 0x8001;                /*enable transfer trigger factor*/
    DTS3 |= 0x01;                  /*enable DMA3*/

    ICDMA3 |= 0x0040;              /*select the way to determine the interrupt vector*/
    ICDMA3 &= 0xEF7F;              /*Enable interrupt*/
}


/******************************************************************************
** Function:    LFDrv_SetDataBuffer
** Description: Set data buffer for DMA transfer
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
boolean LFDrv_SetDataBuffer(uint8 *databuff,uint8 Siz)
{
    uint8 databuffidx;
    boolean rv=FALSE;
    boolean LFDrvContinue=FALSE;
    
    if(LFDrv_GetStatus()==LFDrv_IDLE)
    {
	/*store the communication data to local buffer*/
        for(databuffidx=0;databuffidx<Siz;databuffidx++)
        {
            PwmDataBuffer[PwmDataBufferIdx]=*(databuff+databuffidx);
	    PwmDataBufferIdx++;
        }
	LFDrvContinue=TRUE;
    }

    if(LFDrvContinue==TRUE)
    {
	/*Set RDT to trigger Simultaneous rewrite(TAUD0 ch4 and ch5)*/
        PwmRdtBuffer[0]=0x0030;

	/*reconfigure the communication data*/
        /*First Byte*/
        PwmDataBufferIdx=0;
        /*set period&duty and output logic for per bit*/	
        for(PwmDataBitIdx=0;PwmDataBitIdx<8;PwmDataBitIdx++)
        {
            PwmDutyBuffer[PwmDutyBufferIdx]=PWMDUTY;  /*set bit's pwm duty*/
            PwmPeriodBuffer[PwmPeriodBufferIdx]=PWMPERIOD-1;/*set bit's pwm period*/
	    if(PwmDataBuffer[PwmDataBufferIdx]&0x01) /*judge per bit 0 or 1*/
            {
                PwmTolBuffer[PwmTolBufferIdx]=0x0020;/*PWM Low to High*/
            }
            else
            {
                PwmTolBuffer[PwmTolBufferIdx]=0x0000;/*PWM High to Low*/
            }
            PwmDataBuffer[PwmDataBufferIdx]=PwmDataBuffer[PwmDataBufferIdx]>>1;
            PwmDutyBufferIdx++;
            PwmPeriodBufferIdx++;
            PwmTolBufferIdx++;
        }

        /*Second Byte*/
        PwmDataBufferIdx++;
        /*Set Second Byte's Bit0's pwm duty&period(3Tbit)*/
        PwmDutyBuffer[PwmDutyBufferIdx]=PWMDUTY*3;
        PwmPeriodBuffer[PwmPeriodBufferIdx]=PWMPERIOD*3-1;
        PwmDutyBufferIdx++;
        PwmPeriodBufferIdx++;
        /*Set Second Byte's Bit1's pwm duty&period(1Tbit)*/
        PwmDutyBuffer[PwmDutyBufferIdx]=PWMDUTY;
        PwmPeriodBuffer[PwmPeriodBufferIdx]=PWMPERIOD-1;
        PwmDutyBufferIdx++;
        PwmPeriodBufferIdx++;
        /*Set Second Byte's Bit2's pwm duty&period(2Tbit)*/
        PwmDutyBuffer[PwmDutyBufferIdx]=PWMDUTY*2;
        PwmPeriodBuffer[PwmPeriodBufferIdx]=PWMPERIOD*2-1;
        PwmDutyBufferIdx++;
        PwmPeriodBufferIdx++;
        /*Set Second Byte's Bit3's pwm duty&period(2Tbit)*/
        PwmDutyBuffer[PwmDutyBufferIdx]=PWMDUTY*2;
        PwmPeriodBuffer[PwmPeriodBufferIdx]=PWMPERIOD*2-1;
        PwmDutyBufferIdx++;
        PwmPeriodBufferIdx++;
        /*Set Second Byte's Bit4's pwm duty&period(1Tbit)*/
        PwmDutyBuffer[PwmDutyBufferIdx]=PWMDUTY;
        PwmPeriodBuffer[PwmPeriodBufferIdx]=PWMPERIOD-1;
        PwmDutyBufferIdx++;
        PwmPeriodBufferIdx++;
        /*Set Second Byte's low 5 bits pwm output logic*/
        for(PwmDataBitIdx=0;PwmDataBitIdx<5;PwmDataBitIdx++)
        {
	    if(PwmDataBuffer[PwmDataBufferIdx]&0x01)
            {
                PwmTolBuffer[PwmTolBufferIdx]=0x0020; /*PWM Low to High*/
            }
            else
            {
                PwmTolBuffer[PwmTolBufferIdx]=0x0000; /*PWM High to Low*/
            }
            PwmDataBuffer[PwmDataBufferIdx]=PwmDataBuffer[PwmDataBufferIdx]>>1;
            PwmTolBufferIdx++;
        }

        /*Third Byte to the last Byte*/
        PwmDataBufferIdx++;
        for(databuffidx=0;databuffidx<Siz-2;databuffidx++)
        {
            for(PwmDataBitIdx=0;PwmDataBitIdx<8;PwmDataBitIdx++)
            {
                PwmDutyBuffer[PwmDutyBufferIdx]=PWMDUTY;
                PwmPeriodBuffer[PwmPeriodBufferIdx]=PWMPERIOD-1;
	        if(PwmDataBuffer[PwmDataBufferIdx]&0x01)
                {
                    PwmTolBuffer[PwmTolBufferIdx]=0x0020;/*PWM Low to High*/
                }
                else
                {
                    PwmTolBuffer[PwmTolBufferIdx]=0x0000;/*PWM High to Low*/
                }
                PwmDataBuffer[PwmDataBufferIdx]=PwmDataBuffer[PwmDataBufferIdx]>>1;
                PwmDutyBufferIdx++;
                PwmPeriodBufferIdx++;
                PwmTolBufferIdx++;
            }
	    PwmDataBufferIdx++;
        }

        /*Last Dummy Bit set PIN LOW*/
        PwmDutyBuffer[PwmDutyBufferIdx]=0;
        PwmPeriodBuffer[PwmPeriodBufferIdx]=99;
        PwmTolBuffer[PwmTolBufferIdx]=0x0000;
        PwmDutyBufferIdx++;
        PwmPeriodBufferIdx++;
        PwmTolBufferIdx++;
        PwmDutyBuffer[PwmDutyBufferIdx]=0;
        PwmPeriodBuffer[PwmPeriodBufferIdx]=99;
        PwmTolBuffer[PwmTolBufferIdx]=0x0000;
        /*end*/
	LFDrvContinue=TRUE;
    }

    /*Configure DMA for PWM data transfer*/
    if(LFDrvContinue==TRUE)
    {
        LFDrv_DmaConfig();
	rv=TRUE;
    }
    return rv;
}


/******************************************************************************
** Function:    LFDrv_ClearDataBuffer
** Description: Clear the data buffer
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
void LFDrv_ClearDataBuffer(void)
{
    uint8 idx;
    for(idx=0;idx<22;idx++)
    {
        PwmDataBuffer[idx]=0;
    }
    for(idx=0;idx<180;idx++)
    {
        PwmPeriodBuffer[idx]=0;
        PwmDutyBuffer[idx]=0;
        PwmTolBuffer[idx]=0;
    }
}


/******************************************************************************
** Function:    LFDrv_RestCmd
** Description: Reset the PwmDataBufferIdx,PwmDutyBufferIdx,
**                        PwmPeriodBufferIdx,PwmTolBufferIdx
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
void LFDrv_RestCmd(void)
{
    PwmDataBufferIdx=0;
    PwmDutyBufferIdx=0;
    PwmPeriodBufferIdx=0;
    PwmTolBufferIdx=0;
}


/******************************************************************************
** Function:    LFDrv_AsyncTrsmStart
** Description: Start the TAU timer to transmit PWM
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
void LFDrv_AsyncTrsmStart(void)
{
    PwmTrsmState=LFDrvIsBusy;/*Set state to Busy*/

    TAUD0TOL &= 0xDFFF; /*postive logic*/
    TAUD0CDR4 = 99;
    TAUD0CDR5 = 0;  /*duty=0%*/
    TAUD0RDT |= 0x0030;
    TAUD0TS |= 0x0030;  /*start ch4 and ch5*/
}


/******************************************************************************
** Function:    LFDrv_AsyncTrsmStop
** Description: Stop the TAU timer to transmit PWM
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
void LFDrv_AsyncTrsmStop(void)
{
    TAUD0TT |= 0x0030; /*stop ch12 and ch13*/
    
    /*TAUD0O5 set Low*/
    TAUD0TOE &= 0xFFDF;
    TAUD0TO &= 0xFFDF;
    TAUD0TOE |= 0x0020;
    PwmTrsmState=LFDrvIsIdle;
}


/******************************************************************************
** Function:    LFDrv_GetStatus
** Description: get the transmit status
** Parameter:   
**              
**              
**              
** Return:      LFDrvRetState
******************************************************************************/
LFDrvStatusReturnType LFDrv_GetStatus(void)
{
    LFDrvStatusReturnType LFDrvRetState;
    if(PwmTrsmState == LFDrvIsIdle)
        LFDrvRetState=LFDrv_IDLE;
    else
        LFDrvRetState=LFDrv_BUSY;

    return LFDrvRetState;
}


/******************************************************************************
** Function:    LFDrv_DMACmpInterrupt
** Description: DMA completed interrupt service function
** Parameter:   
**              
**              
**              
** Return:      None
******************************************************************************/
void LFDrv_DMACmpInterrupt(void)
{
    TAUD0TT |= 0x0030; /*stop the counter*/
    DRQCLR |= 0x000F;  /*Clear transfer request flag*/
    DTS0 &= 0x7F;      /*Clear transfer completed flag*/
    DTS1 &= 0x7F;      /*Clear transfer completed flag*/
    DTS2 &= 0x7F;      /*Clear transfer completed flag*/
    DTS3 &= 0x7F;      /*Clear transfer completed flag*/
    PwmTrsmState=LFDrvIsIdle;
}

