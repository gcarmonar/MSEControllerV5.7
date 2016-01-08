/**
 * @file comm.c
 * @author Jalil Chavez
 * @date 03 May 2015
 * @brief File containing communication protocol between matlab
 * and microcontroller.
 *
 * @see http://www.ti.com/lit/ug/sprufz5a/sprufz5a.pdf
 */

/*
 * Host-uC Communications
 *
 */

#include <DSP2833x_Device.h>     // DSP2833x Headerfile Include File
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <OSAPI.h>				// OS API
#include <MSECConfig.h>			// MSE Controller global configuration

#include "conin.h"
#include "comm.h"					// Comm API

/* Module level configuration */

#define COMM_Input			SYS_ControlInputSource
#define COMM_PosInput1		SYS_PosInput1
#define COMM_PosInput2		SYS_PosInput2
#define COMM_SP				SYS_SetPoint
#define COMM_N				SYS_N
#define COMM_pfa 			SYS_a
#define COMM_pfb 			SYS_b
#define COMM_pfu 			SYS_u
#define COMM_pfy 			SYS_y
#define COMM_ControlDomain	SYS_ControlDomain
#define COMM_SamplingPeriod	SYS_SamplingPeriod
#define COMM_MSECState		SYS_MSECState


static void COMM__vRxHandler( void );
static void COMM__vTxHandler( void );
static void COMM__vDecodeMessage( COMM_tunMsg* punMsg );
static COMM__vValidateData( COMM_tunMsg* punMsg );
static COMM__vFillBlankSpacesInMemory( COMM_tunMsg* punMsg );
static COMM_tenCmdId COMM__enFindCommand( COMM_tunMsg* punMsg );
/**
 * ================================================================ Command functions START
 */
void COMM__vSetCoeffsNum01( COMM_tunMsg* punMsg );
void COMM__vSetCoeffsNum23( COMM_tunMsg* punMsg );
void COMM__vSetCoeffsNum45( COMM_tunMsg* punMsg );
void COMM__vSetCoeffsNum67( COMM_tunMsg* punMsg );
void COMM__vSetCoeffsNum89( COMM_tunMsg* punMsg );
void COMM__vSetCoeffsNum1011( COMM_tunMsg* punMsg );
void COMM__vSetCoeffsDen01( COMM_tunMsg* punMsg );
void COMM__vSetCoeffsDen23( COMM_tunMsg* punMsg );
void COMM__vSetCoeffsDen45( COMM_tunMsg* punMsg );
void COMM__vSetCoeffsDen67( COMM_tunMsg* punMsg );
void COMM__vSetCoeffsDen89( COMM_tunMsg* punMsg );
void COMM__vSetCoeffsDen1011( COMM_tunMsg* punMsg );
void COMM__vControllerSetup( COMM_tunMsg* punMsg );
void COMM__vStartStopController( COMM_tunMsg* punMsg );
void COMM__vReadDataPos( COMM_tunMsg* punMsg );
void COMM__vReadDataVel( COMM_tunMsg* punMsg );
void COMM__vTestConnection( COMM_tunMsg* punMsg );


/**
 * ================================================================ Command functions END
 */
static COMM_tstMsgDesc COMM__astCmdList[ COMM_nNoOfCMDs ] = { COMM__nCmdCfg };
static const Uint8 COMM__ru8CmdConversionTbl[ COMM_nNoOfCMDs ] = { COMM_nCmdId2CmdVal };
static COMM__tenStatesSM COMM__enState = COMM_nIdle;
static Uint8 COMM__au8RxTxBuffer[ COMM_nRxBufferLength ] = { 0 };
static Uint8 COMM__au8BURxTxBuffer[ COMM_nTxBufferLength ] = { 0 };

/**
 * COMM_vInit
 * @param void
 * @returns void
 */
void COMM_vInit( void )
{
	UART_vInit();
	COMM__enState = COMM_nIdle;
	/**
	 * Open com port, and move SM to COMM_nOpened state
	 */
	UART_vOpen( UART_nCh0, &COMM__vRxHandler, &COMM__vTxHandler);
	COMM__enState = COMM_nOpened;
}

/**
 * COMM__vRxHandler
 * @param void
 * @returns void
 */
static void COMM__vRxHandler( void )
{

	COMM__enState = COMM_nReceiving;
	UART_enRead( UART_nCh0, &COMM__au8RxTxBuffer[0], COMM_nFrameLength );
	memcpy(&COMM__au8BURxTxBuffer[0],&COMM__au8RxTxBuffer[0],COMM_nFrameLength);
	COMM__vDecodeMessage( (COMM_tunMsg*)(&COMM__au8RxTxBuffer[0]) );
}

/**
 * COMM__vTxHandler
 * @param void
 * @returns void
 */
static void COMM__vTxHandler( void )
{
	COMM__enState = COMM_nSending;
}

/**
 * COMM__vDecodeMessage
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
static void COMM__vDecodeMessage( COMM_tunMsg* punMsg )
{
	COMM_tenCmdId enCmd;

	enCmd = COMM__enFindCommand( punMsg );
	COMM__astCmdList[ enCmd ].punMsg = punMsg;
	COMM__vValidateData( punMsg );
	switch( COMM__enState )
	{
		case COMM_nIdle:

			break;
		case COMM_nOpened:

			break;
		case COMM_nReceiving:
			COMM__vFillBlankSpacesInMemory( punMsg );
			if( COMM__astCmdList[ enCmd ].pvCmdHdlr != (tpvCMDHdlr)(0) )
			{
					COMM__astCmdList[ enCmd ].pvCmdHdlr( punMsg );
			}
			break;
		case COMM_nSending:

			break;
		case COMM_nError:

			break;
		default:

			break;

	}
}

/**
 * COMM__vFillBlankSpacesInMemory
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
static COMM__vFillBlankSpacesInMemory( COMM_tunMsg* punMsg )
{
	punMsg->au8Byte[2] = ((punMsg->au8Byte[3]<<8)&0xFF00) | (punMsg->au8Byte[2]&0x00FF);
	punMsg->au8Byte[3] = ((punMsg->au8Byte[5]<<8)&0xFF00) | (punMsg->au8Byte[4]&0x00FF);
	punMsg->au8Byte[4] = ((punMsg->au8Byte[7]<<8)&0xFF00) | (punMsg->au8Byte[6]&0x00FF);
	punMsg->au8Byte[5] = ((punMsg->au8Byte[9]<<8)&0xFF00) | (punMsg->au8Byte[8]&0x00FF);
	punMsg->au8Byte[6] = ((punMsg->au8Byte[11]<<8)&0xFF00) | (punMsg->au8Byte[10]&0x00FF);
	punMsg->au8Byte[7] = ((punMsg->au8Byte[13]<<8)&0xFF00) | (punMsg->au8Byte[12]&0x00FF);
}

/**
 * COMM__vValidateData
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
static COMM__vValidateData( COMM_tunMsg* punMsg )
{
	if( (punMsg->stMsgElem.u8SOF == COMM_nSOF) &&
		(punMsg->stMsgElem.u8EOF == COMM_nEOF) )
	{

	}
	else
	{
		COMM__enState = COMM_nError;
	}
}

/**
 * COMM__enFindCommand
 * @param COMM_tunMsg* punMsg
 * @returns COMM_tenCmdId 
 */
static COMM_tenCmdId COMM__enFindCommand( COMM_tunMsg* punMsg )
{
	COMM_tenCmdId enIdx;

	for( enIdx = (COMM_tenCmdId)(0); enIdx < COMM_nNoOfCMDs; enIdx++ )
	{
		if( COMM__ru8CmdConversionTbl[ enIdx ] == (COMM_tenCmdId)(punMsg->stMsgElem.u8CMD) )
		{
			break;
		}
	}

	if( enIdx == COMM_nNoOfCMDs )
	{
		COMM__enState = COMM_nError;
	}

	return enIdx;
}
/**================================================================================================
 *										COMMANDS( START )
 *================================================================================================*/
 /**
 * COMM__vTestConnection
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vTestConnection( COMM_tunMsg* punMsg )
{
	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

 /**
 * COMM__vSetCoeffsNum01
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsNum01( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfb[ 0 ] = f1;
	COMM_pfb[ 1 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

 /**
 * COMM__vSetCoeffsNum23
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsNum23( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfb[ 2 ] = f1;
	COMM_pfb[ 3 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

 /**
 * COMM__vSetCoeffsNum45
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsNum45( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfb[ 4 ] = f1;
	COMM_pfb[ 5 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

 /**
 * COMM__vSetCoeffsNum67
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsNum67( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfb[ 6 ] = f1;
	COMM_pfb[ 7 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

 /**
 * COMM__vSetCoeffsNum89
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsNum89( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfb[ 8 ] = f1;
	COMM_pfb[ 9 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

 /**
 * COMM__vSetCoeffsNum1011
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsNum1011( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfb[ 10 ] = f1;
	COMM_pfb[ 11 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

 /**
 * COMM__vSetCoeffsDen01
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsDen01( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfa[ 0 ] = f1;
	COMM_pfa[ 1 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

/**
 * COMM__vSetCoeffsDen23
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsDen23( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfa[ 2 ] = f1;
	COMM_pfa[ 3 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

/**
 * COMM__vSetCoeffsDen45
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsDen45( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfa[ 4 ] = f1;
	COMM_pfa[ 5 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

/**
 * COMM__vSetCoeffsDen67
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsDen67( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfa[ 6 ] = f1;
	COMM_pfa[ 7 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

/**
 * COMM__vSetCoeffsDen89
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsDen89( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfa[ 8 ] = f1;
	COMM_pfa[ 9 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

/**
 * COMM__vSetCoeffsDen1011
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vSetCoeffsDen1011( COMM_tunMsg* punMsg )
{
	static float32 f1 = 0.0;
	static float32 f2 = 0.0;
	f1 = (float32)punMsg->stData.fData1;
	f2 = (float32)punMsg->stData.fData2;
	COMM_pfa[ 10 ] = f1;
	COMM_pfa[ 11 ] = f2;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

/**
 * COMM__vControllerSetup
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vControllerSetup( COMM_tunMsg* punMsg )
{
	static Uint32 u32Val1 = 0;
	static float32 fVal2 = 0;
	u32Val1 = (Uint32)(punMsg->u32Data.u32Data2);
	fVal2 = (punMsg->stData.fData3);
	COMM_Input = (MSEC_enInputSensor)(punMsg->au8Byte[2]);
	COMM_SamplingPeriod = u32Val1;
	COMM_SP = (float32)(fVal2);

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

/**
 * COMM__vStartStopController
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vStartStopController( COMM_tunMsg* punMsg )
{
	if( (punMsg->stMsgElem.u8CMD) ==  COMM__ru8CmdConversionTbl[ COMM_nCMD_StartCtrlr ] )
	{
		CONIN_vSetEQEPInitVal();
		COMM_MSECState = MSEC_enRunning;
		COMM_ControlDomain = MSEC_enZ;
	}
	else if( (punMsg->stMsgElem.u8CMD) ==  COMM__ru8CmdConversionTbl[ COMM_nCMD_StopCtrlr ] )
	{
		CONOUT_vInit();
		COMM_MSECState = MSEC_enSetup;
		COMM_ControlDomain = MSEC_enZ;
	}
	else
	{
		COMM_MSECState = MSEC_enInvalid;
	}

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8BURxTxBuffer[0]), COMM_nFrameLength );
}

/**
 * COMM__vReadDataPos
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vReadDataPos( COMM_tunMsg* punMsg )
{
	union
	{
	float32 fTemp;
	Uint16 au16[2];
	} unTemp;

	memcpy(&(unTemp.fTemp),&COMM_PosInput1,sizeof(COMM_PosInput1));

	punMsg->au8Byte[2] = 0x000000FF & unTemp.au16[0];
	punMsg->au8Byte[3] = (0x0000FF00 & unTemp.au16[0])>>8;
	punMsg->au8Byte[4] = 0x000000FF & unTemp.au16[1];
	punMsg->au8Byte[5] = (0x0000FF00 & unTemp.au16[1])>>8;

	memcpy(&(unTemp.fTemp),&COMM_PosInput2,sizeof(COMM_PosInput2));

	punMsg->au8Byte[6] = 0x000000FF & unTemp.au16[0];
	punMsg->au8Byte[7] = (0x0000FF00 & unTemp.au16[0])>>8;
	punMsg->au8Byte[8] = 0x000000FF & unTemp.au16[1];
	punMsg->au8Byte[9] = (0x0000FF00 & unTemp.au16[1])>>8;

	/**
	 * Send response
	 */
	UART_enWrite( UART_nCh0, &(COMM__au8RxTxBuffer[0]), COMM_nFrameLength );
}

/**
 * COMM__vReadDataVel
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM__vReadDataVel( COMM_tunMsg* punMsg )
{
/*	punMsg->au8Byte[2] = (Uint8)((0x000000FF & (Uint32)(COMM_PosInput1)));
	punMsg->au8Byte[3] = (Uint8)((0x0000FF00 & (Uint32)(COMM_PosInput1))>>8);
	punMsg->au8Byte[4] = (Uint8)((0x00FF0000 & (Uint32)(COMM_PosInput1))>>16);
	punMsg->au8Byte[5] = (Uint8)((0xFF000000 & (Uint32)(COMM_PosInput1))>>24);
	punMsg->au8Byte[6] = (Uint8)((0x000000FF & (Uint32)(COMM_PosInput2)));
	punMsg->au8Byte[7] = (Uint8)((0x0000FF00 & (Uint32)(COMM_PosInput2))>>8);
	punMsg->au8Byte[8] = (Uint8)((0x00FF0000 & (Uint32)(COMM_PosInput2))>>16);
	punMsg->au8Byte[9] = (Uint8)((0xFF000000 & (Uint32)(COMM_PosInput2))>>24);
*/
	/**
	 * Send response
	 */
	//UART_enWrite( UART_nCh0, &(COMM__au8RxTxBuffer[0]), COMM_nFrameLength );
}

/*================================================================================================
 *										COMMANDS( END )
 *================================================================================================*/

/** 
* Main function task 
*/

/**
 * COMM_vTask
 * @param COMM_tunMsg* punMsg
 * @returns void
 */
void COMM_vTask(void) {

	for (;;)
	{
		/** System state machine */
		switch(COMM_MSECState)
		{
			case MSEC_enSetup:

				break;
			case MSEC_enRunning:


				break;
			case MSEC_enReporting:


				break;
			default:
				/** error condition */
				break;
		}

		OS_SLEEP((Uint32)(COMM_SamplingPeriod));
	}
}




