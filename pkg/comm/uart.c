/**
 * @file uart.c
 * @author Jalil Chavez
 * @date 26 April 2015
 * @brief File containing uart low level driver.
 *
 * @see http://www.ti.com/lit/ug/sprufz5a/sprufz5a.pdf
 */
 
/**
 * Foreign header files
 */
#include <DSP2833x_Device.h>     // DSP2833x Headerfile Include File
/**
 * Internal header files
 */
 #include "uart.h"
 #include <string.h>

/**
 * Local functions
 **/

static void UART__vInitInterrupts( void );
static void UART__vDeinitInterrupts( void );
static void UART__vInitGpio( void );
static void UART__vFifoInit( void );
static void UART__vFifoDeinit( void );
static void UART__vPtrShift( Uint8 *pu8Array, Uint8 u8CopyLength, Uint8 u8CopyPoint);

#if( UART__nCh0Enable == Yes )
	static void UART__vInitCh0Gpio( void );
	static void UART__vFifoCh0Init( void );
	static void UART__vFifoCh0Deinit( void );

	#if ( UART__nRxISREnable == Yes )
		interrupt void UART__vRxFifoCh0Isr( void );
	#endif

	#if ( UART__nTxISREnable == Yes )
		interrupt void UART__vTxFifoCh0Isr( void );
	#endif
#endif


#if( UART__nCh1Enable == Yes )
	static void UART__vInitCh1Gpio( void );
	static void UART__vFifoCh1Init( void );
	static void UART__vFifoCh1Deinit( void );

	#if ( UART__nRxISREnable == Yes )
		interrupt void UART__vRxFifoCh1Isr( void );
	#endif

	#if ( UART__nTxISREnable == Yes )
		interrupt void UART__vTxFifoCh1Isr( void );
	#endif
#endif

#if( UART__nCh2Enable == Yes )
static void UART__vInitCh2Gpio( void );
static void UART__vFifoCh2Init( void );
static void UART__vFifoCh2Deinit( void );
#if ( UART__nRxISREnable == Yes )
interrupt void UART__vRxFifoCh2Isr( void );
#endif

#if ( UART__nTxISREnable == Yes )
interrupt void UART__vTxFifoCh2Isr( void );
#endif

#endif

/**
 * Array of UART channels containing configuration and status parameters for each one.
 */
UART_tstChannelDesc UART__astChannelDesc[UART_nNoOfChannels] =	UART__nChannelsInitialCfg;

/**
 * Tx buffer for all UART channels.
 */
Uint8 UART__au8TxBuffer[UART_nNoOfChannels][UART__nBufferLength] = UART__nChannelsInitTxBuff;

/**
 * Rx buffer for all UART channels.
 */
Uint8 UART__au8RxBuffer[UART_nNoOfChannels][UART__nBufferLength] = UART__nChannelsInitRxBuff;


/**
 * Initializes the port functionality.
 * @param void
 * @returns void
 */
 void UART_vInit( void )
 {
	 UART__vInitGpio();
 }

/**
 * Opens an UART channel.
 * This function intialices a Serial port with the parameters specified
 * by the channel descriptor.
 * @param pstChannelDesc
 * @param pvRxHdlr
 * @returns void
 */
 void UART_vOpen( UART_tenChannelID enChID, tNotificationFunc pvRxHdlr,  tNotificationFunc pvTxHdlr )
 {
	  UART__vInitInterrupts();
	  UART__astChannelDesc[enChID].enStat = UART_nOpened;
	  UART__astChannelDesc[enChID].pvRxHandler = pvRxHdlr;
	  UART__astChannelDesc[enChID].pvTxHandler = pvTxHdlr;
 }

 /**
  *	Description: Close an UART channel.
  *	If a channel is closed, all transmission is stopped.
  *	The UART channel state is set back to UART__nChX_Cfg.
  *	@param enCh: channel number to be closed
  *	@returns void
  */
  void UART_vClose( UART_tenChannelID enChID )
  {
	  UART__vDeinitInterrupts();
	  UART__astChannelDesc[enChID].enStat = UART_nClosed;
  }

  /**
   * Description: Read data from an UART channel rx buffer.
   * Read is only valid, when data is available in rx buffer or rx
   * notification callback is triggered.
   * @return UART_tenRetValue
   * @param enChID:       channel ID
   * @param puq6RxBuf:    pointer to buffer where data will be retrieved
   * @param u8RxDataLen: length of data for reception
   */
#pragma CODE_SECTION(UART_enRead, "funsec")
  UART_tenRetValue UART_enRead (UART_tenChannelID enChID, Uint8 *pu8RxBuf, Uint8 u8RxDataLen)
  {
	  UART_tenRetValue enRetVal;
	  UART__vEnterCriticalSection();
	  if( UART__astChannelDesc[enChID].enStat == UART_nOpened && UART__astChannelDesc[enChID].u8RxDataAvailable >= u8RxDataLen)
	  {
		  memcpy(pu8RxBuf,&UART__au8RxBuffer[enChID][0],u8RxDataLen*sizeof(Uint8));
		  UART__vPtrShift(&UART__au8RxBuffer[enChID][0],UART__astChannelDesc[UART_nCh0].u8RxDataAvailable - u8RxDataLen, u8RxDataLen);
		  UART__astChannelDesc[UART_nCh0].u8RxDataAvailable = UART__astChannelDesc[UART_nCh0].u8RxDataAvailable - u8RxDataLen;
		  enRetVal = UART_nOk;
		  UART__vLeaveCriticalSection();
	  }
	  else
	  {
		  enRetVal = UART_nError;
		  UART__vLeaveCriticalSection();
	  }
	  return enRetVal;
  }

  /**
   * Description: Returns the value of data available in RX channel buffer.
   * @return Uint8
   * @param UART_tenChannelID enChID
   */
  Uint8 UART_vGetAvailableRxData( UART_tenChannelID enChID )
  {
	  return  UART__astChannelDesc[enChID].u8RxDataAvailable;
  }

  /**
   * Description: Returns the value of data available in TX channel buffer.
   * @return Uint8
   * @param UART_tenChannelID enChID
   */
  Uint8 UART_vGetAvailableTxData( UART_tenChannelID enChID )
  {
	  return  UART__astChannelDesc[enChID].u8TxDataAvailable;
  }

  /**
   * Description: Write data to an UART channel tx buffer.
   * Write is only valid, when buss is free or tx
   * notification callback is triggered.
   * @return UART_tenRetValue
   * @param enChID:       channel ID
   * @param puq6RxBuf:    pointer to buffer where data will be obtained
   * @param u8RxDataLen: length of data for transmission
   */
#pragma CODE_SECTION(UART_enWrite, "funsec")
  UART_tenRetValue UART_enWrite (UART_tenChannelID enChID, Uint8 *pu8TxBuf, Uint8 u8TxDataLen)
  {
	  UART_tenRetValue enRetVal;
	  UART__vEnterCriticalSection();
	  if( UART__astChannelDesc[enChID].enStat == UART_nOpened && UART__astChannelDesc[enChID].u8TxDataAvailable == 0 )
	  {
		  memcpy(&UART__au8TxBuffer[enChID][0],pu8TxBuf,u8TxDataLen*sizeof(Uint8));
		  UART__astChannelDesc[enChID].u8TxDataAvailable = u8TxDataLen;
		  enRetVal = UART_nOk;
		  UART__vLeaveCriticalSection();
	  }
	  else
	  {
		  enRetVal = UART_nError;
		  UART__vLeaveCriticalSection();
	  }

	  #if( UART__nTxISREnable == No )
	  	  Uint8 u8BuffIdx;
	  	  for(u8BuffIdx = 0; u8BuffIdx < ( UART__astChannelDesc[enChID].u8TxDataAvailable ); u8BuffIdx++)
  	  	  {
	  		  switch( enChID )
	  		  {
				#if( UART__nCh0Enable == Yes )
	  		  	  case  UART_nCh0:
	  		  		  SciaRegs.SCITXBUF = UART__au8TxBuffer[enChID][u8BuffIdx];
					  break;
				#endif
				#if( UART__nCh1Enable == Yes )
	  		  	  case  UART_nCh1:
	  		  		  ScibRegs.SCITXBUF = UART__au8TxBuffer[enChID][u8BuffIdx];
					  break;
				#endif
				#if( UART__nCh2Enable == Yes )
	  		  	  case  UART_nCh2:
	  		  		  ScicRegs.SCITXBUF = UART__au8TxBuffer[enChID][u8BuffIdx];
					  break;
				#endif
				  default:
					  break;
	  		  }
  	  	  }
	  	  if( UART__astChannelDesc[enChID].pvTxHandler != NULL )
	  	  {
	  		switch( enChID )
	  		{
				#if( UART__nCh0Enable == Yes )
				  case  UART_nCh0:
					  while(SciaRegs.SCIFFTX.bit.TXFFST != 0);
					  break;
				#endif
				#if( UART__nCh1Enable == Yes )
				  case  UART_nCh1:
					  while(ScibRegs.SCIFFTX.bit.TXFFST != 0);
					  break;
				#endif
				#if( UART__nCh2Enable == Yes )
				  case  UART_nCh2:
					  while(ScicRegs.SCIFFTX.bit.TXFFST != 0);
					  break;
				#endif
	  		}
	  		  UART__astChannelDesc[enChID].u8TxDataAvailable = UART__astChannelDesc[enChID].u8TxDataAvailable - u8TxDataLen;
	  		  UART__astChannelDesc[enChID].pvTxHandler();
	  	  }
	  	  else
	  	  {
			/**
			 * Do nothing
			 */
	  	  }
	  #else



	  #endif

	  return enRetVal;
  }

  /**
   * Description: This function copies
   * u8CopyLength elements from u8CopyPoint to array base.
   * @return void
   * @param Uint8 *pu8Array
   * @param Uint8 u8CopyLength
   * @param Uint8 u8CopyPoint
   */
#pragma CODE_SECTION(UART__vPtrShift, "funsec")
static  void UART__vPtrShift(Uint8 *pu8Array, Uint8 u8CopyLength, Uint8 u8CopyPoint)
  {
     Uint8 u8Idx;

     for (u8Idx = 0; u8Idx < (u8CopyLength ); u8Idx++)
     {
    	 pu8Array[ u8Idx ] = pu8Array[ u8CopyPoint + u8Idx ];
     }
  }

  /**
   * Description: Initializes gpios used by UART channels. Function
   * configures channels enabled in configuration section( uart.h ).
   * @return void
   * @param void
   */
  static void UART__vInitGpio()
  {
	#if( UART__nCh0Enable == Yes )
		 UART__vInitCh0Gpio();
	#endif
	#if( UART__nCh1Enable == Yes )
		 UART__vInitCh1Gpio();
	#endif
	#if( UART__nCh2Enable == Yes )
		 UART__vInitCh2Gpio();
	#endif
  }

  /**
   * Description: Initializes interrupts used by UART channels.
   * 			  Depeding on configurations in uart.h.
   * @return void
   * @param void
   */
#pragma CODE_SECTION(UART__vInitInterrupts, "funsec")
static void UART__vInitInterrupts( void )
  {
	  /**
	   * Disable interrupts
	   */
	  UART__vEnterCriticalSection();
	  /**
	   * If Interrupts are configured it is necessary
	   * to assign a service funftion
	   */
	  UART__vInitPIECtrl();
	  UART__vInitPIEVecTable();

	  /**
	   * If Interrupts are configured it is necessary
	   * to assign a service funftion
	   */

	  	/**
	  	 * ================================= CH0 */
	  	#if( (UART__nRxISREnable == Yes) && ( UART__nCh0Enable == Yes ) )
	  		UART__vAssignRxCh0Fun2IRQVector( UART__vRxFifoCh0Isr );
	  	#endif/* ( UART__nRxISREnable == Yes ) */
	  	#if( (UART__nTxISREnable == Yes) && ( UART__nCh0Enable == Yes ) )
	  		UART__vAssignTxCh0Fun2IRQVector( UART__vTxFifoCh0Isr );
	  	#endif/* ( UART__nTxISREnable == Yes ) */
	  	/**
	  	 * ================================= CH1 */
	  	#if( (UART__nRxISREnable == Yes) && ( UART__nCh1Enable == Yes ) )
	  		UART__vAssignRxCh1Fun2IRQVector( UART__vRxFifoCh1Isr );
	  	#endif/* ( UART__nRxISREnable == Yes ) */
	  	#if( (UART__nTxISREnable == Yes) && ( UART__nCh1Enable == Yes ) )
	  		UART__vAssignTxCh1Fun2IRQVector( UART__vTxFifoCh1Isr );
	  	#endif/* ( UART__nTxISREnable == Yes ) */
	  	/**
	  	 * ================================= CH2 */
	  	#if( (UART__nRxISREnable == Yes) && ( UART__nCh2Enable == Yes ) )
	  		UART__vAssignRxCh2Fun2IRQVector( UART__vRxFifoCh2Isr );
	  	#endif/* ( UART__nRxISREnable == Yes ) */
	  	#if( (UART__nTxISREnable == Yes) && ( UART__nCh2Enable == Yes ) )
	  		UART__vAssignTxCh2Fun2IRQVector( UART__vTxFifoCh2Isr );
	  	#endif/* ( UART__nTxISREnable == Yes ) */

	  	UART__vFifoInit();

		/**
		* Enable interrupts
		*/
	  	UART__vLeaveCriticalSection();
  }

  /**
    * Description: Deinitializes interrupts used by UART channels.
    * 			  Depeding on configurations in uart.h.
    * @return void
    * @param void
    */
#pragma CODE_SECTION(UART__vDeinitInterrupts, "funsec")
static void UART__vDeinitInterrupts( void )
   {

 	  UART__vEnterCriticalSection();
 	  UART__vFifoDeinit();
 	  UART__vLeaveCriticalSection();
   }

  /**
   * Description: Initilalizes the FIFO for each channel considering rx/tx buffer length
   * @return void
   * @param void
   */
static void UART__vFifoInit( void )
{
#if( UART__nCh0Enable == Yes )
	  UART__vFifoCh0Init();
#endif
#if( UART__nCh1Enable == Yes )
	  UART__vFifoCh1Init();
#endif
#if( UART__nCh2Enable == Yes )
	  UART__vFifoCh2Init();
#endif
}

/**
 * Description: Initilalizes the FIFO for each channel considering rx/tx buffer length
 * @return void
 * @param void
 */
static void UART__vFifoDeinit( void )
{
#if( UART__nCh0Enable == Yes )
	UART__vFifoCh0Deinit();
#endif
#if( UART__nCh1Enable == Yes )
	  UART__vFifoCh1Deinit();
#endif
#if( UART__nCh2Enable == Yes )
	  UART__vFifoCh2Deinit();
#endif
}

#if( UART__nCh0Enable == Yes )
/**
 * Description: Initializes gpios of SCI-A
 * @return void
 * @param void
 */
static void UART__vInitCh0Gpio( void )
  {
    EALLOW;

  	GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;    /**< Enable pull-up for GPIO28 (SCIRXDA) */
  	GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;	   /**< Enable pull-up for GPIO29 (SCITXDA) */
  	GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;  /**< Asynch input GPIO28 (SCIRXDA) */
  	GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;   /**< Configure GPIO28 for SCIRXDA operation */
  	GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;   /**< Configure GPIO29 for SCITXDA operation */

  	EDIS;
  }

  /**
   * Description: Initializes gpios of SCI-A
   * @return void
   * @param void
   */
static void UART__vFifoCh0Init( void )
  {
  	SciaRegs.SCICCR.all = 0x0007;		/**< 1 stop bit,  No loopback, No parity,8 char bits, async mode, idle-line protocol */
  	SciaRegs.SCICTL1.all = 0x0003;
  	SciaRegs.SCICTL2.bit.TXINTENA = UART__astChannelDesc[UART_nCh0].stCfg.boTxEn;
  	SciaRegs.SCICTL2.bit.RXBKINTENA = UART__astChannelDesc[UART_nCh0].stCfg.boRxEn;
  	SciaRegs.SCIHBAUD    = ((UART__astChannelDesc[UART_nCh0].stCfg.u16BaudRate) & 0xFF00) >> 8;
  	SciaRegs.SCILBAUD    = ((UART__astChannelDesc[UART_nCh0].stCfg.u16BaudRate) & 0x00FF);
  	SciaRegs.SCICCR.bit.LOOPBKENA = 0;
  	SciaRegs.SCIFFTX.all = 0xC020 | UART__astChannelDesc[UART_nCh0].stCfg.u8TxFifoLenght;
  	SciaRegs.SCIFFRX.all = 0x0020 | UART__astChannelDesc[UART_nCh0].stCfg.u8RxFifoLenght;
  	SciaRegs.SCIFFCT.all = 0x00;
  	SciaRegs.SCICTL1.all = 0x0023;
  	SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 1;
  	SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;

	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   /**< Enable the PIE block */
	PieCtrlRegs.PIEIER9.bit.INTx1 = UART__astChannelDesc[UART_nCh0].stCfg.boRxEn;     /**< PIE Group 9, INT1 */
	PieCtrlRegs.PIEIER9.bit.INTx2 = UART__astChannelDesc[UART_nCh0].stCfg.boTxEn;     /**< PIE Group 9, INT2 */
	IER = 0x100;						 /**< Enable CPU INT */
  }

  /**
   * Description: Deitializes gpios of SCI-A
   * @return void
   * @param void
   */
static void UART__vFifoCh0Deinit( void )
  {
	SciaRegs.SCICTL2.bit.TXINTENA = 0;
	SciaRegs.SCICTL2.bit.RXBKINTENA = 0;
	SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 0;
	SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   /**< Enable the PIE block */
	PieCtrlRegs.PIEIER9.bit.INTx1=1;     /**< PIE Group 9, INT1 */
	PieCtrlRegs.PIEIER9.bit.INTx2=0;     /**< PIE Group 9, INT2 */
	IER = 0x100;						 /**< Enable CPU INT */
  }

#endif

#if( UART__nCh1Enable == Yes )
  /**
   * Description: Initializes gpios of SCI-B
   * @return void
   * @param void
   */
static void UART__vInitCh1Gpio( void )
  {
    EALLOW;

    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;	   /**< Enable pull-up for GPIO18 (SCITXDB) */
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;	   /**< Enable pull-up for GPIO19 (SCIRXDB) */
  	GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3;  /**< Asynch input GPIO19 (SCIRXDB) */
  	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 2;   /**< Configure GPIO18 for SCITXDB operation */
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 2;   /**< Configure GPIO19 for SCIRXDB operation */

    EDIS;
  }

  /**
   * Description: Initializes gpios of SCI-B
   * @return void
   * @param void
   */
static void UART__vFifoCh1Init( void )
  {
  	ScibRegs.SCICCR.all = 0x0007;		/**< 1 stop bit,  No loopback, No parity,8 char bits, async mode, idle-line protocol */
  	ScibRegs.SCICTL1.all = 0x0003;
  	ScibRegs.SCICTL2.bit.TXINTENA = UART__astChannelDesc[UART_nCh1].stCfg.boTxEn;
  	ScibRegs.SCICTL2.bit.RXBKINTENA = UART__astChannelDesc[UART_nCh1].stCfg.boRxEn;
  	ScibRegs.SCIHBAUD    = ((UART__astChannelDesc[UART_nCh1].stCfg.u16BaudRate) & 0xFF00) >> 8;
  	ScibRegs.SCILBAUD    = ((UART__astChannelDesc[UART_nCh1].stCfg.u16BaudRate) & 0x00FF);
  	ScibRegs.SCICCR.bit.LOOPBKENA = 0;
  	ScibRegs.SCIFFTX.all = 0xC020 | UART__astChannelDesc[UART_nCh1].stCfg.u8TxFifoLenght;
  	ScibRegs.SCIFFRX.all = 0x0020 | UART__astChannelDesc[UART_nCh1].stCfg.u8RxFifoLenght;
  	ScibRegs.SCIFFCT.all = 0x00;
  	ScibRegs.SCICTL1.all = 0x0023;
  	ScibRegs.SCIFFTX.bit.TXFIFOXRESET = 1;
  	ScibRegs.SCIFFRX.bit.RXFIFORESET = 1;

	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   /**< Enable the PIE block */
	PieCtrlRegs.PIEIER9.bit.INTx3 = UART__astChannelDesc[UART_nCh1].stCfg.boRxEn;     /**< PIE Group 9, INT3 */
	PieCtrlRegs.PIEIER9.bit.INTx4 = UART__astChannelDesc[UART_nCh1].stCfg.boTxEn;     /**< PIE Group 9, INT4 */
	IER = 0x100;						 /**< Enable CPU INT */
  }

  /**
   * Description: Deitializes gpios of SCI-A
   * @return void
   * @param void
   */
static void UART__vFifoCh1Deinit( void )
  {
	ScibRegs.SCICTL2.bit.TXINTENA = 0;
	ScibRegs.SCICTL2.bit.RXBKINTENA = 0;
	ScibRegs.SCIFFTX.bit.TXFIFOXRESET = 0;
	ScibRegs.SCIFFRX.bit.RXFIFORESET = 0;
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   /**< Enable the PIE block */
	PieCtrlRegs.PIEIER9.bit.INTx3=1;     /**< PIE Group 9, INT1 */
	PieCtrlRegs.PIEIER9.bit.INTx4=1;     /**< PIE Group 9, INT2 */
	IER = 0x100;						 /**< Enable CPU INT */
  }
#endif

#if( UART__nCh2Enable == Yes )
  /**
   * Description: Initializes gpios of SCI-C
   * @return void
   * @param void
   */
static void UART__vInitCh2Gpio( void )
  {
	EALLOW;

	GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0;    /**< Enable pull-up for GPIO62 (SCIRXDC) */
  	GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;	   /**< Enable pull-up for GPIO63 (SCITXDC) */
  	GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;  /**< Asynch input GPIO62 (SCIRXDC) */
  	GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;   /**< Configure GPIO62 for SCIRXDC operation */
  	GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;   /**< Configure GPIO63 for SCITXDC operation */

  	EDIS;
  }
  /**
   * Description: Initializes gpios of SCI-C
   * @return void
   * @param void
   */
static void UART__vFifoCh2Init( void )
  {
  	ScicRegs.SCICCR.all = 0x0007;		/**< 1 stop bit,  No loopback, No parity,8 char bits, async mode, idle-line protocol */
  	ScicRegs.SCICTL1.all = 0x0003;
  	ScicRegs.SCICTL2.bit.TXINTENA = UART__astChannelDesc[UART_nCh2].stCfg.boTxEn;
  	ScicRegs.SCICTL2.bit.RXBKINTENA = UART__astChannelDesc[UART_nCh2].stCfg.boRxEn;
  	ScicRegs.SCIHBAUD    = ((UART__astChannelDesc[UART_nCh2].stCfg.u16BaudRate) & 0xFF00) >> 8;
  	ScicRegs.SCILBAUD    = ((UART__astChannelDesc[UART_nCh2].stCfg.u16BaudRate) & 0x00FF);
  	ScicRegs.SCICCR.bit.LOOPBKENA = 0;
  	ScicRegs.SCIFFTX.all= 0xC020 | UART__astChannelDesc[UART_nCh2].stCfg.u8TxFifoLenght;
  	ScicRegs.SCIFFRX.all= 0x0028 | UART__astChannelDesc[UART_nCh2].stCfg.u8RxFifoLenght;
  	ScicRegs.SCIFFCT.all= 0x00;
  	ScicRegs.SCICTL1.all = 0x0023;
  	ScicRegs.SCIFFTX.bit.TXFIFOXRESET = 1;
  	ScicRegs.SCIFFRX.bit.RXFIFORESET = 1;

	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   /**< Enable the PIE block */
	PieCtrlRegs.PIEIER8.bit.INTx5 = UART__astChannelDesc[UART_nCh2].stCfg.boRxEn;     /**< PIE Group 5, INT5 */
	PieCtrlRegs.PIEIER8.bit.INTx6 = UART__astChannelDesc[UART_nCh2].stCfg.boTxEn;     /**< PIE Group 6, INT6 */
	IER = 0x100;						 /**< Enable CPU INT */
  }

  /**
   * Description: Deitializes gpios of SCI-A
   * @return void
   * @param void
   */
static void UART__vFifoCh2Deinit( void )
  {
	SciaRegs.SCICTL2.bit.TXINTENA = 0;
	SciaRegs.SCICTL2.bit.RXBKINTENA = 0;
	SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 0;
	SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   /**< Enable the PIE block */
	PieCtrlRegs.PIEIER9.bit.INTx1=1;     /**< PIE Group 9, INT1 */
	PieCtrlRegs.PIEIER9.bit.INTx2=1;     /**< PIE Group 9, INT2 */
	IER = 0x100;						 /**< Enable CPU INT */
  }
#endif

  /**
   * Description: Tx FIFO called every time that
   * FIFO is empty.
   * @return void
   * @param void
   */
#if ( (UART__nTxISREnable == Yes) && ( UART__nCh0Enable == Yes ) )
interrupt void UART__vTxFifoCh0Isr( void )
{
	Uint8 u8BuffIdx;

	for(u8BuffIdx = 0; u8BuffIdx < ( UART__astChannelDesc[UART_nCh0].u8TxDataAvailable ); u8BuffIdx++)
	{
		SciaRegs.SCITXBUF = UART__au8TxBuffer[UART_nCh0][u8BuffIdx];
	}

	if( UART__astChannelDesc[UART_nCh0].pvTxHandler != NULL )
	{
		UART__astChannelDesc[enChID].u8TxDataAvailable = 0;
		UART__astChannelDesc[UART_nCh0].pvTxHandler();
	}
	else
	{
		/**
		 * Do nothing
		 */
	}


	SciaRegs.SCIFFTX.bit.TXFFINTCLR=1;
	PieCtrlRegs.PIEACK.all|=0x100;
}
#endif

	/**
	 * Description: Tx FIFO called every time that
	 * FIFO is empty.
	 * @return void
	 * @param void
	 */
#if ( (UART__nRxISREnable == Yes) && ( UART__nCh0Enable == Yes ) )
interrupt void UART__vRxFifoCh0Isr( void )
{
	Uint8 u8BuffIdx;
	Uint8 u8BufferedDataCnt = 0;
	Uint8 u8InitiDataCnt = UART__astChannelDesc[UART_nCh0].u8RxDataAvailable;

	for(u8BuffIdx = u8InitiDataCnt; u8BuffIdx < (u8InitiDataCnt + UART__nFifoLength); u8BuffIdx++)
	{
		if( u8BuffIdx < UART__nBufferLength )
		{
			UART__au8RxBuffer[UART_nCh0][u8BuffIdx] = SciaRegs.SCIRXBUF.all;	 /**< Read data*/
			u8BufferedDataCnt++;
		}
		else
		{
			/**
			 * Data has not been read yet, and buffer is full.
			 */
			UART__astChannelDesc[UART_nCh0].enStat = UART_nFault;
		}
	}

	UART__astChannelDesc[UART_nCh0].u8RxDataAvailable = UART__astChannelDesc[UART_nCh0].u8RxDataAvailable + u8BufferedDataCnt;

	if( UART__astChannelDesc[UART_nCh0].pvRxHandler != NULL )
	{
		UART__astChannelDesc[UART_nCh0].pvRxHandler();
	}
	else
	{
		/**
		 * Do nothing
		 */
	}

	SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;  /**< Clear Overflow flag */
	SciaRegs.SCIFFRX.bit.RXFFINTCLR=1; 	/**< Clear Interrupt flag */
	PieCtrlRegs.PIEACK.all|=0x100;  	/**< Issue PIE ack*/
}
#endif

	/**
	 * Description: Tx FIFO called every time that
	 * FIFO is empty.
	 * @return void
	 * @param void
	 */
#if ( (UART__nTxISREnable == Yes) && ( UART__nCh1Enable == Yes ) )
interrupt void UART__vTxFifoCh1Isr( void )
{
	Uint8 u8BuffIdx;

	for(u8BuffIdx = 0; u8BuffIdx < ( UART__astChannelDesc[UART_nCh1].u8TxDataAvailable ); u8BuffIdx++)
	{
		ScibRegs.SCITXBUF = UART__au8TxBuffer[UART_nCh1][u8BuffIdx];
	}

	if( UART__astChannelDesc[UART_nCh1].pvTxHandler != NULL )
	{
		UART__astChannelDesc[enChID].u8TxDataAvailable = 0;
		UART__astChannelDesc[UART_nCh1].pvTxHandler();
	}
	else
	{
		/**
		 * Do nothing
		 */
	}


	ScibRegs.SCIFFTX.bit.TXFFINTCLR=1;
	PieCtrlRegs.PIEACK.all |= 0x100;
}
#endif

	/**
	 * Description: Tx FIFO called every time that
	 * FIFO is empty.
	 * @return void
	 * @param void
	 */
#if ( (UART__nRxISREnable == Yes) && ( UART__nCh1Enable == Yes ) )
interrupt void UART__vRxFifoCh1Isr( void )
{
	Uint8 u8BuffIdx;
	Uint8 u8BufferedDataCnt = 0;
	Uint8 u8InitiDataCnt = UART__astChannelDesc[UART_nCh1].u8RxDataAvailable;

	for(u8BuffIdx = u8InitiDataCnt; u8BuffIdx < (u8InitiDataCnt + UART__nFifoLength); u8BuffIdx++)
	{
		if( u8BuffIdx < UART__nBufferLength )
		{
			UART__au8RxBuffer[UART_nCh1][u8BuffIdx] = SciaRegs.SCIRXBUF.all;	 /**< Read data*/
			u8BufferedDataCnt++;
		}
		else
		{
			/**
			 * Data has not been read yet, and buffer is full.
			 */
			UART__astChannelDesc[UART_nCh1].enStat = UART_nFault;
		}
	}

	UART__astChannelDesc[UART_nCh1].u8RxDataAvailable = UART__astChannelDesc[UART_nCh1].u8RxDataAvailable + u8BufferedDataCnt;

	if( UART__astChannelDesc[UART_nCh1].pvRxHandler != NULL )
	{
		UART__astChannelDesc[UART_nCh1].pvRxHandler();
	}
	else
	{
		/**
		 * Do nothing
		 */
	}

	ScibRegs.SCIFFRX.bit.RXFFOVRCLR=1;  /**< Clear Overflow flag */
	ScibRegs.SCIFFRX.bit.RXFFINTCLR=1; 	/**< Clear Interrupt flag */
	PieCtrlRegs.PIEACK.all|=0x100;  	/**< Issue PIE ack*/
}
#endif

/**
 * Description: Tx FIFO called every time that
 * FIFO is empty.
 * @return void
 * @param void
 */
#if ( (UART__nTxISREnable == Yes) && ( UART__nCh2Enable == Yes ) )
interrupt void UART__vTxFifoCh2Isr( void )
{
	Uint8 u8BuffIdx;

	for(u8BuffIdx = 0; u8BuffIdx < ( UART__astChannelDesc[UART_nCh2].u8TxDataAvailable ); u8BuffIdx++)
	{
		ScicRegs.SCITXBUF = UART__au8TxBuffer[UART_nCh2][u8BuffIdx];
	}

	if( UART__astChannelDesc[UART_nCh2].pvTxHandler != NULL )
	{
		UART__astChannelDesc[enChID].u8TxDataAvailable = 0;
		UART__astChannelDesc[UART_nCh2].pvTxHandler();
	}
	else
	{
		/**
		 * Do nothing
		 */
	}


	ScicRegs.SCIFFTX.bit.TXFFINTCLR=1;
	PieCtrlRegs.PIEACK.all |= 0x100;
}
#endif

/**
 * Description: Tx FIFO called every time that
 * FIFO is empty.
 * @return void
 * @param void
 */
#if ( (UART__nRxISREnable == Yes) && ( UART__nCh2Enable == Yes ) )
interrupt void UART__vRxFifoCh2Isr( void )
{
	Uint8 u8BuffIdx;
	Uint8 u8BufferedDataCnt = 0;
	Uint8 u8InitiDataCnt = UART__astChannelDesc[UART_nCh2].u8RxDataAvailable;

	for(u8BuffIdx = u8InitiDataCnt; u8BuffIdx < (u8InitiDataCnt + UART__nFifoLength); u8BuffIdx++)
	{
		if( u8BuffIdx < UART__nBufferLength )
		{
			UART__au8RxBuffer[UART_nCh2][u8BuffIdx] = SciaRegs.SCIRXBUF.all;	 /**< Read data*/
			u8BufferedDataCnt++;
		}
		else
		{
			/**
			 * Data has not been read yet, and buffer is full.
			 */
			UART__astChannelDesc[UART_nCh2].enStat = UART_nFault;
		}
	}

	UART__astChannelDesc[UART_nCh2].u8RxDataAvailable = UART__astChannelDesc[UART_nCh2].u8RxDataAvailable + u8BufferedDataCnt;

	if( UART__astChannelDesc[UART_nCh2].pvRxHandler() != NULL )
	{
		UART__astChannelDesc[UART_nCh2].pvRxHandler();
	}
	else
	{
		/**
		 * Do nothing
		 */
	}

	ScicRegs.SCIFFRX.bit.RXFFOVRCLR=1;  /**< Clear Overflow flag */
	ScicRegs.SCIFFRX.bit.RXFFINTCLR=1; 	/**< Clear Interrupt flag */
	PieCtrlRegs.PIEACK.all|=0x100;  	/**< Issue PIE ack*/
}
#endif
