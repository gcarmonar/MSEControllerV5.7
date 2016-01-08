/**
 * @file conout.c
 * @author  
 * @date  
 * @brief  
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
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <OSAPI.h>				// OS API
#include <MSECConfig.h>			// MSE Controller global configuration

#include "conout.h"					// ConOut API

/**
 * Module level configuration 
 */

#define CONOUT_SP			SYS_SetPoint
#define CONOUT_N			SYS_N
#define CONOUT_pfy 			SYS_y
#define CONOUT_TaskPeriod	SYS_SamplingPeriod
#define CONOUT_MSECState	SYS_MSECState


#define CONOUT_MaxVal		(float32)(4.0)
#define CONOUT_MinVal		(float32)(-4.0)
#define CONOUT_DacZero		0x80
#define CONOUT_DacRes		0xFF
#define CONOUT_ScaleFactor	100
#define CONOUT_DacSteps		(Uint16)(2*CONOUT_MaxVal*CONOUT_ScaleFactor)

/**
 * Module level variables 
 */ 

static float32	CONOUT_fDACout = 0;                          	/**< DAC output variable*/
Uint32	CONOUT_u32DACout = 0;                          	/**< DAC output variable*/

/**
 * Module initialization
 * @param void
 * @returns void
 */ 
void CONOUT_vInit(void)
{
	EALLOW;

	GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;     /**<DAC 3*/
	GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;     /**<DAC 4*/
	GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;     /**<DAC 5*/
	GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;     /**<DAC 6*/
	GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;     /**<DAC 7*/
	GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;     /**<DAC 8*/
	GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;     /**<DAC 9*/
	GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;     /**<DAC 10*/

	/**
	 * Initi port values( 0x80 )
	 */
	GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
	GpioDataRegs.GPASET.bit.GPIO7 = 1;

	/**
	 * Init as outputs
	 */
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
	GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;

	EDIS;
}

/**
 * COMM__vProcessOutput
 * @param void
 * @returns void
 */   
void COMM__vProcessOutput( void )
{
	if(CONOUT_MSECState == MSEC_enRunning)
	{
		if( CONOUT_pfy[0] > CONOUT_MaxVal )
		{
			CONOUT_fDACout = CONOUT_MaxVal;				/**< Limit output to max value*/
		}
		else if( CONOUT_pfy[0] < CONOUT_MinVal )
		{
			CONOUT_fDACout = CONOUT_MinVal;				/**< Limit output to min value*/
		}
		else
		{
			CONOUT_fDACout = CONOUT_pfy[0];				/**< Output inside allowed range*/
		}

		/**
		 *  Convert float controller out to Uint16
		 */
		CONOUT_u32DACout = (Uint32)( ( CONOUT_fDACout + CONOUT_MaxVal ) * CONOUT_ScaleFactor );

		/**
		 *  Scale output
		 */
		CONOUT_u32DACout = (CONOUT_u32DACout*CONOUT_DacRes)/( CONOUT_DacSteps );
		/** 
		* TODO: This must be uncommented when output is necessary
		*/
		//CONOUT_u32DACout = (0x000000FF) & CONOUT_u32DACout;
		CONOUT_u32DACout = 0x80;
		if( (CONOUT_u32DACout & 0x01) == 0x01 )
		{
			GpioDataRegs.GPASET.bit.GPIO0 = 1;
		}
		else
		{
			GpioDataRegs.GPACLEAR.bit.GPIO0 = 1;
		}

		if( ((CONOUT_u32DACout>>1) & 0x01) == 0x01 )
		{
			GpioDataRegs.GPASET.bit.GPIO1 = 1;
		}
		else
		{
			GpioDataRegs.GPACLEAR.bit.GPIO1 = 1;
		}

		if( ((CONOUT_u32DACout>>2) & 0x01) == 0x01 )
		{
			GpioDataRegs.GPASET.bit.GPIO2 = 1;
		}
		else
		{
			GpioDataRegs.GPACLEAR.bit.GPIO2 = 1;
		}

		if( ((CONOUT_u32DACout>>3) & 0x01) == 0x01 )
		{
			GpioDataRegs.GPASET.bit.GPIO3 = 1;
		}
		else
		{
			GpioDataRegs.GPACLEAR.bit.GPIO3 = 1;
		}

		if( ((CONOUT_u32DACout>>4) & 0x01) == 0x01 )
		{
			GpioDataRegs.GPASET.bit.GPIO4 = 1;
		}
		else
		{
			GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
		}

		if( ((CONOUT_u32DACout>>5) & 0x01) == 0x01 )
		{
			GpioDataRegs.GPASET.bit.GPIO5 = 1;
		}
		else
		{
			GpioDataRegs.GPACLEAR.bit.GPIO5 = 1;
		}

		if( ((CONOUT_u32DACout>>6) & 0x01) == 0x01 )
		{
			GpioDataRegs.GPASET.bit.GPIO6 = 1;
		}
		else
		{
			GpioDataRegs.GPACLEAR.bit.GPIO6 = 1;
		}

		if( ((CONOUT_u32DACout>>7) & 0x01) == 0x01 )
		{
			GpioDataRegs.GPASET.bit.GPIO7 = 1;
		}
		else
		{
			GpioDataRegs.GPACLEAR.bit.GPIO7 = 1;
		}


		/** y(0) is written in the output (ePWM) */
	}
}

/**
 * Main function task
 * @param void
 * @returns void
 */
void CONOUT_vTask(void)
{
	for (;;)
	{
		COMM__vProcessOutput();
		OS_SLEEP((Uint32)(CONOUT_TaskPeriod));
	}
}




