/**
 * @file uart.h
 * @author Jalil Chavez
 * @date 26 April 2015
 * @brief File containing communication protocol between matlab
 * and microcontroller.
 *
 * @see http://www.ti.com/lit/ug/sprufz5a/sprufz5a.pdf
 */
 
#ifndef _UART_H_
#define	_UART_H_

#include <DSP2833x_GlobalPrototypes.h>
#include "..\cdef\cdef.h"

/**
*===============================================================
*					Adaption macros
*===============================================================
*/

/**
  Disable interrupts
 */
#define UART__vEnterCriticalSection()	DINT

/**
  Enable interrupts
 */
#define UART__vLeaveCriticalSection()	EINT

/**
  Initialize PIE control registers
 */
#define UART__vInitPIECtrl()			InitPieCtrl();\
										IER = 0x0000;\
										IFR = 0x0000

/**
  Initialize PIE Vector table
 */
#define UART__vInitPIEVecTable()		InitPieVectTable()

#define UART__vAssignRxCh0Fun2IRQVector(a)	\
										EALLOW;\
										PieVectTable.SCIRXINTA = &(a);\
										EDIS

#define UART__vAssignTxCh0Fun2IRQVector(a)	\
										EALLOW;\
										PieVectTable.SCITXINTA = &(a);\
										EDIS

#define UART__vAssignRxCh1Fun2IRQVector(a)	\
										EALLOW;\
										PieVectTable.SCIRXINTB = &(a);\
										EDIS

#define UART__vAssignTxCh1Fun2IRQVector(a)	\
										EALLOW;\
										PieVectTable.SCITXINTB = &(a);\
										EDIS

#define UART__vAssignRxCh2Fun2IRQVector(a)	\
										EALLOW;\
										PieVectTable.SCIRXINTC = &(a);\
										EDIS

#define UART__vAssignTxCh2Fun2IRQVector(a)	\
										EALLOW;\
										PieVectTable.SCITXINTC = &(a);\
										EDIS
/**
*===============================================================
*					Configuration parameters
*===============================================================
*/

/**
  Enable communications using ISR for Tx channel
 */
#define UART__nTxISREnable			(No)

/**
  Enable communications using ISR for Rx channel
 */
#define UART__nRxISREnable			(Yes)

/**
  Buffer length, this value is used for all channels in module
 */
#define UART__nBufferLength			(32)

/**
  Fifo size, it is the length of the hardware buffer
 */
#define UART__nFifoLength			(15)


/**
  Initial configuration for channel 0
 */

/**
 * RX - GPIO28
 * TX - GPIO29
 */
#define UART__nCh0Enable			(Yes)
#if( UART__nCh0Enable == Yes )
#define UART__nCh0_Cfg				{	UART_nClosed, 		/**< enStat */\
										0,					/**< u16RxDataAvailable */\
										0,					/**< u16TxDataAvailable */\
										NULL,				/**< pu16RxBuffer */\
										NULL,				/**< pu16TxBuffer */\
										NULL,				/**< pvRxHandler */\
										NULL,				/**< pvTxHandler */\
										UART_nBaud115200,	/**< u16BaudRate */\
										UART__nFifoLength,	/**< u16RxFifoLenght */\
										UART__nFifoLength,	/**< u16TxFifoLenght */\
										TRUE,				/**< boRxEn */\
										FALSE				/**< boTxEn */\
									},
#define UART__nCh0RxBuffInit		{0},
#define UART__nCh0TxBuffInit		{0},
#else
#define UART__nCh0_Cfg
#define UART__nCh0RxBuffInit
#define UART__nCh0TxBuffInit
#endif

/**
  Initial configuration for channel 1
 */

/**
 * RX - GPIO19
 * TX - GPIO18
 */
#define UART__nCh1Enable			(No)
#if( UART__nCh1Enable == Yes )
#define UART__nCh1_Cfg				{	UART_nClosed, 		/**< enStat */\
										0,					/**< u16RxDataAvailable */\
										0,					/**< u16TxDataAvailable */\
										NULL,				/**< pu16RxBuffer */\
										NULL,				/**< pu16TxBuffer */\
										NULL,				/**< pvRxHandler */\
										NULL,				/**< pvTxHandler */\
										UART_nBaud115200,	/**< u16BaudRate */\
										UART__nFifoLength,	/**< u16RxFifoLenght */\
										UART__nFifoLength,	/**< u16TxFifoLenght */\
										TRUE,				/**< boRxEn */\
										FALSE				/**< boTxEn */\
									},
#define UART__nCh1RxBuffInit		{0},
#define UART__nCh1TxBuffInit		{0},
#else
#define UART__nCh1_Cfg
#define UART__nCh1RxBuffInit
#define UART__nCh1TxBuffInit
#endif

/**
  Initial configuration for channel 2
 */
#define UART__nCh2Enable			(No)
#if( UART__nCh2Enable == Yes )
#define UART__nCh2_Cfg				{	UART_nClosed, 		/**< enStat */\
										0,					/**< u16RxDataAvailable */\
										0,					/**< u16TxDataAvailable */\
										NULL,				/**< pu16RxBuffer */\
										NULL,				/**< pu16TxBuffer */\
										NULL,				/**< pvRxHandler */\
										NULL,				/**< pvTxHandler */\
										UART_nBaud115200,	/**< u16BaudRate */\
										UART__nFifoLength,	/**< u16RxFifoLenght */\
										UART__nFifoLength,	/**< u16TxFifoLenght */\
										TRUE,				/**< boRxEn */\
										FALSE				/**< boTxEn */\
									}
#define UART__nCh2RxBuffInit		{0},
#define UART__nCh2TxBuffInit		{0},
#else
#define UART__nCh2_Cfg
#define UART__nCh2RxBuffInit
#define UART__nCh2TxBuffInit
#endif

#define UART__nChannelsInitialCfg	{ UART__nCh0_Cfg UART__nCh1_Cfg UART__nCh2_Cfg }
#define UART__nChannelsInitRxBuff	{ UART__nCh0RxBuffInit UART__nCh1RxBuffInit UART__nCh2RxBuffInit}
#define UART__nChannelsInitTxBuff	{ UART__nCh0TxBuffInit UART__nCh1TxBuffInit UART__nCh2TxBuffInit}

#if( (UART__nCh0Enable == No) && (UART__nCh1Enable == No) && (UART__nCh2Enable == No)  )
#error "UART: At least one channel must be enabled"
#endif

/**
  Pre defined values to use different communication baud rates
 */
#define    UART_nBaudDummy		(Uint16)(0)
#define    UART_nBaud9600		(Uint16)(487)
#define    UART_nBaud19200		(Uint16)(243)
#define    UART_nBaud38400		(Uint16)(181)
#define    UART_nBaud57600		(Uint16)(80)
#define    UART_nBaud115200		(Uint16)(39)

/**
*===============================================================
*					Module types
*===============================================================
*/

/**
  An enumeration of UART channels
 */
typedef enum
{
#if( UART__nCh0Enable == Yes )
	UART_nCh0 = 0,				/**< UART Channel 0*/
#endif
#if( UART__nCh1Enable == Yes )
	UART_nCh1,					/**< UART Channel 1*/
#endif
#if( UART__nCh2Enable == Yes )
	UART_nCh2,					/**< UART Channel 2*/
#endif
	UART_nNoOfChannels			/**< Max number of channels*/
}UART_tenChannelID;
/**
  An enumeration of UART process result
 */
typedef enum
{
	UART_nOk = 0,				/**< Ok result of any operation performed by UART module*/
	UART_nError,				/**< Erroneous result of any operation performed by UART module*/
	UART_nNumOfResp				/**< Count of ennumeration elements*/
} UART_tenRetValue;


/**
  An enumeration of UART channel status
 */
typedef enum
{
	UART_nOpened = 0,			/**< Channel opened*/
	UART_nClosed,				/**< Channel closed, initial status of any port*/
	UART_nBusy,					/**< Ok result of any operation performed by UART module*/
	UART_nFault					/**< Eror detected in channel*/
} UART_tenChannelState;


/**
  Function pointer type(used for handling transmission and reception of data)
 */
typedef void (*tNotificationFunc) ( void );


typedef struct
{
	const Uint16 u16BaudRate;
	const Uint8 u8RxFifoLenght;
	const Uint8 u8TxFifoLenght;
	const Uint8 boRxEn	:	1;
	const Uint8 boTxEn	:	1;
}UART_tstChannelCfg;

typedef struct
{
	UART_tenChannelState enStat;
	Uint8 u8RxDataAvailable;
	Uint8 u8TxDataAvailable;
	Uint8* pu8RxBuffer;
	Uint8* pu8TxBuffer;
	tNotificationFunc pvRxHandler;
	tNotificationFunc pvTxHandler;
	UART_tstChannelCfg stCfg;
} UART_tstChannelDesc;

/**
*===============================================================
*					Function prototypes
*===============================================================
*/

void UART_vInit( void );
void UART_vOpen( UART_tenChannelID enChID, tNotificationFunc pvRxHdlr,  tNotificationFunc pvTxHdlr );
void UART_vClose( UART_tenChannelID enChID );
UART_tenRetValue UART_enRead (UART_tenChannelID enChID, Uint8 *pu16RxBuf, Uint8 u16RxDataLen);
Uint8 UART_vGetAvailableRxData( UART_tenChannelID enChID );
UART_tenRetValue UART_enWrite (UART_tenChannelID enChID, Uint8 *pu16TxBuf, Uint8 u16TxDataLen);
Uint8 UART_vGetAvailableTxData( UART_tenChannelID enChID );

#endif /* _UART_H_ */
