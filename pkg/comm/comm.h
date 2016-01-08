/**
 * @file comm.h
 * @author Jalil Chavez
 * @date 03 May 2015
 * @brief File containing communication protocol between matlab
 * and microcontroller.
 *
 * @see http://www.ti.com/lit/ug/sprufz5a/sprufz5a.pdf
 */

#ifndef _COMM_H_
#define _COMM_H_

#include "..\cdef\cdef.h"
#include "uart.h"

/**
*===============================================================
*					Adaption macros
*===============================================================
*/

#define COMM_nSOF			10
#define COMM_nEOF			13

/**
*===============================================================
*					Configuration parameters
*===============================================================
*/

#define COMM_nFrameLength				UART__nFifoLength

#define COMM_nRxBufferLength			UART__nBufferLength
#define COMM_nTxBufferLength			UART__nBufferLength

#define COMM_nDataPerFrame				11

#define COMM_nCmdId2CmdVal				\
											1,		/**< COMM_nCMD_TstCon */	     	\
										   18,		/**< COMM_nCMD_Setup */	 	     	\
										   20,		/**< COMM_nCMD_Nums0s1 */     		\
										   22,		/**< COMM_nCMD_Nums2s3 */	 		\
										   24,		/**< COMM_nCMD_Nums4s5 */	 		\
										   26,		/**< COMM_nCMD_Nums6s7 */	 		\
										   28,		/**< COMM_nCMD_Nums8s9 */	 		\
										   30,		/**< COMM_nCMD_Nums10s11 */	 		\
										   32,		/**< COMM_nCMD_Dens0s1 */   		\
										   34,		/**< COMM_nCMD_Dens2s3 */			\
										   36,		/**< COMM_nCMD_Dens4s5 */			\
										   38,		/**< COMM_nCMD_Dens6s7 */			\
										   40,		/**< COMM_nCMD_Dens8s9 */			\
										   42,		/**< COMM_nCMD_Dens10s11 */			\
										  100,   	/**< COMM_nCMD_ReadDataEnc1 */	    \
                                          102,  	/**< COMM_nCMD_ReadDataEnc2 */	    \
                                          200,  	/**< COMM_nCMD_StartCtrlr*/		    \
										  201,  	/**< COMM_nCMD_StopCtrlr */

#define COMM__nCmdCfg																		\
										{													\
											COMM_nCMD_TstCon,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vTestConnection,		/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_Setup,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vControllerSetup,	/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_Nums0s1,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsNum01,		/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_Nums2s3,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsNum23,		/**< pvCmdHdlr */	\
										},													\
										{													\
                                        	COMM_nCMD_Nums4s5,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsNum45,		/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_Nums6s7,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsNum67,		/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_Nums8s9,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsNum89,		/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_Nums10s11,		/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsNum1011,		/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_Dens0s1,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsDen01,		/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_Dens2s3,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsDen23,		/**< pvCmdHdlr */	\
										},													\
										{													\
                                        	COMM_nCMD_Dens4s5,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsDen45,		/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_Dens6s7,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsDen67,		/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_Dens8s9,			/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsDen89,		/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_Dens10s11,		/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vSetCoeffsDen1011,		/**< pvCmdHdlr */	\
										},	                                                \
										{													\
											COMM_nCMD_ReadDataPos,		/**< enCmdId */		\
       										(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vReadDataPos,		/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_ReadDataVel,		/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vReadDataVel,			/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_StartCtrlr,		/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vStartStopController,/**< pvCmdHdlr */	\
										},													\
										{													\
											COMM_nCMD_StopCtrlr,		/**< enCmdId */		\
											(COMM_tunMsg*)(0),			/**< punMsg */		\
											&COMM__vStartStopController,/**< pvCmdHdlr */	\
										}
/**
*===============================================================
*					Module types
*===============================================================
*/

typedef enum
{
	COMM_nIdle = 0,
	COMM_nOpened,
	COMM_nSending,
	COMM_nReceiving,
	COMM_nError,
	COMM_nNoOfStates
}COMM__tenStatesSM ;

typedef enum
{
	COMM_nCMD_TstCon = 0,
	COMM_nCMD_Setup,
	COMM_nCMD_Nums0s1,
	COMM_nCMD_Nums2s3,
	COMM_nCMD_Nums4s5,
	COMM_nCMD_Nums6s7,
	COMM_nCMD_Nums8s9,
	COMM_nCMD_Nums10s11,
	COMM_nCMD_Dens0s1,
	COMM_nCMD_Dens2s3,
	COMM_nCMD_Dens4s5,
	COMM_nCMD_Dens6s7,
	COMM_nCMD_Dens8s9,
	COMM_nCMD_Dens10s11,
	COMM_nCMD_ReadDataPos,
	COMM_nCMD_ReadDataVel,
	COMM_nCMD_StartCtrlr,
	COMM_nCMD_StopCtrlr,
	COMM_nNoOfCMDs
} COMM_tenCmdId;

typedef enum
{
	COMM_nFloat32 = 0,
	COMM_nFloat64,
	COMM_nU16,
	COMM_nI16,
	COMM_nU32,
	COMM_nI32
}COMM_tenDataType;

typedef struct
{
	Uint8 u8SOF;
	Uint8 u8CMD;
	Uint8 u8DataByte1;
	Uint8 u8DataByte2;
	Uint8 u8DataByte3;
	Uint8 u8DataByte4;
	Uint8 u8DataByte5;
	Uint8 u8DataByte6;
	Uint8 u8DataByte7;
	Uint8 u8DataByte8;
	Uint8 u8DataByte9;
	Uint8 u8DataByte10;
	Uint8 u8DataByte11;
	Uint8 u8DataByte12;
	Uint8 u8EOF;
} COMM__tstMsgElem;

typedef struct
{
	Uint8 u8Padding1;
	Uint8 u8Padding2;
	float32 fData1;
	float32 fData2;
	float32 fData3;
	Uint8 u8Padding3;
}COMM_tstData;

typedef struct
{
	Uint8 u8Padding1;
	Uint8 u8Padding2;
	Uint32 u32Data1;
	Uint32 u32Data2;
	Uint32 u32Data3;
	Uint8 u8Padding3;
}COMM_tstDatau32;

typedef union
{
	COMM__tstMsgElem stMsgElem;
	COMM_tstData stData;
	COMM_tstDatau32 u32Data;
	Uint8 au8Byte[COMM_nFrameLength];
}COMM_tunMsg;

typedef void (*tpvCMDHdlr) ( COMM_tunMsg* punMsg  );

typedef struct
{
	COMM_tenCmdId enCmdId;
	COMM_tunMsg* punMsg;
	tpvCMDHdlr pvCmdHdlr;
}COMM_tstMsgDesc;

/**
*===============================================================
*					Function prototypes
*===============================================================
*/
void COMM_vInit( void );
void COMM_vTask(void);
#endif	/* _COMM_H_ */
