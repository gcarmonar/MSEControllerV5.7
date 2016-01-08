/**
 * @file  conin.c
 * @author  
 * @date  
 * @brief File containing common definitions for all project.
 *
 */
 /**
 * S-domain Controller
 *
 */
 
/**
 * Foreign header files
 */
#include <DSP2833x_Device.h>     // DSP2833x Headerfile Include File
#include <DSP2833x_GlobalPrototypes.h>
#include <DSP2833x_EQep.h>
#include <DSP2833x_Gpio.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <OSAPI.h>				// OS API
#include <MSECConfig.h>			// MSE Controller global configuration

/**
 * Internal header files
 */
#include "conin.h"					// ConIn API

/**
 * Module level configuration 
 */

#define CONIN_SP					SYS_SetPoint
#define CONIN_N						SYS_N
#define CONIN_pfu					SYS_u
#define CONIN_pfy 					SYS_y
#define CONIN_TaskPeriod			SYS_SamplingPeriod
#define CONIN_ControlInputSource	SYS_ControlInputSource
#define CONIN_PosInput1				SYS_PosInput1
#define CONIN_PosInput2				SYS_PosInput2
#define CONIN_MSECState				SYS_MSECState

#define EQEP__nKEncoder		(float32)(70.6)
#define EQEP__nDivEncoder	(float32)(16000)
#define EQEP__nInitValEnc	0xFFFF
#define EQEP__nOffsetMilim1	(float32)(289.173)
#define EQEP__nOffsetMilim2	(float32)(289.173)

/**
 * Module level variables 
 */ 


/**
 * Local functions
 */
void CONIN__vBufferShift(void);

 
/**
 * Module initialization
 * @param void
 * @returns void
 */
void CONIN_vInit(void)
{
	InitEQep1Gpio();
	InitEQep2Gpio();

}
/**
 * CONIN_vSetEQEPInitVal
 * @param void
 * @returns void
 */
void CONIN_vSetEQEPInitVal( void )
{
	EQep1Regs.QPOSCNT = EQEP__nInitValEnc;
	EQep2Regs.QPOSCNT = EQEP__nInitValEnc;
}

/**
 * Main function task
 * @param void
 * @returns void
 */  
void CONIN_vTask(void) {

	static float u0 = 0;
	static float uIn1 = 0;
	static float uIn2 = 0;
	float new_count1 = 0;
	float new_count2 = 0;

	for (;;)
	{
		if(CONIN_MSECState == MSEC_enRunning)
		{
			new_count1 = EQep1Regs.QPOSCNT;
			
			if((new_count1 > 0x7FFFFFFF) && (new_count1 <= 0xFFFFFFFF))
			{          
			/**
			* ================================= Izquierda */
				new_count1 = new_count1 - EQep1Regs.QPOSMAX;
				uIn1 = ((new_count1*EQEP__nKEncoder)/EQEP__nDivEncoder)-EQEP__nOffsetMilim1;
			}
			else if ((new_count1 < 0x7FFFFFFF) && (new_count1 >= 0))
					{             
					/**
					* ================================= Derecha */
				uIn1 = ((new_count1*EQEP__nKEncoder)/EQEP__nDivEncoder)-EQEP__nOffsetMilim1;
			}


			new_count2 = EQep2Regs.QPOSCNT;

			if((new_count2 > 0x7FFFFFFF) && (new_count2 <= 0xFFFFFFFF))
					{          
					/**
					* ================================= Izquierda */
				new_count2 = new_count2 - EQep2Regs.QPOSMAX;
				uIn2 = ((new_count2*EQEP__nKEncoder)/EQEP__nDivEncoder)-EQEP__nOffsetMilim2;
			}
			else if ((new_count2 < 0x7FFFFFFF) && (new_count2 >= 0))
					{             
					/**
					* ================================= Derecha */
				uIn2 = ((new_count2*EQEP__nKEncoder)/EQEP__nDivEncoder)-EQEP__nOffsetMilim2;
				uIn2 = -1.0*uIn2;/** TODO: Figure out why sign is different between Enc1 and Enc2 */
			}
			switch(SYS_ControlInputSource)
			{
				case MSEC_enInput1:
					u0 = uIn1;
					break;
				case MSEC_enInput2:
					u0 = uIn2;
					break;
				default:
					/**
					* ================================= wrong condition */
					break;
			}

			/** 
			* u() and y() buffers shifting 
			*/
			CONIN__vBufferShift();

			/** 
			* u(0) update 
			*/
			CONIN_PosInput1 = uIn1;
			CONIN_PosInput2 = uIn2;
			u0 = CONIN_SP - u0;
			CONIN_pfu[0] = u0;
		}

		OS_SLEEP((Uint32)(CONIN_TaskPeriod));
	}
}

/**
 * CONIN__vBufferShift
 * @param void
 * @returns void
 */  
void CONIN__vBufferShift(void)
{
	unsigned int i = 0;

	if(CONIN_N <= 1)
	{
		/**
		 * error, N shall be higher than 1!
		 */ 
		return;
	}

	 /**
	 * size u[] = N + 1
	 */ 
	for(i = CONIN_N; i >= 1; i--)
	{
		/**
		 * u(i) = u(i-1)
		 */  
		CONIN_pfu[i] = CONIN_pfu[i-1];
	}
	/**
	 * u(0) becomes free
	 */ 
	CONIN_pfu[0] = 0;

	/**
	 * size y[] = N + 1
	 */ 
	for(i = CONIN_N; i >= 1; i--)
	{
		/**
		 * y(i) = y(i-1)
		 */ 
		CONIN_pfy[i] = CONIN_pfy[i-1];
	}
	/**
	 * y(0) becomes free
	 */ 
	CONIN_pfy[0] = 0;
}




