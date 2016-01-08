/**
 * @file uart.c
 * @author 
 * @date 
 * @brief  
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

/**
 * Internal header files
 */
#include "scon.h"					// SCon API

/**
 * Module level configuration 
 */

#define SCON_SP				SYS_SetPoint
#define SCON_Car			SYS_CarSelect
#define SCON_N				SYS_N
#define SCON_pfa 			SYS_a
#define SCON_pfb 			SYS_b
#define SCON_pfu 			SYS_u
#define SCON_pfy 			SYS_y
#define SCON_ControlDomain	SYS_ControlDomain
#define SCON_TaskPeriod		SYS_SamplingPeriod
#define SCON_MSECState		SYS_MSECState

/**
 * Module level variables 
 */  

 /**
 * Module initialization
 * @param void
 * @returns void
 */
void SCON_vInit(void)
{

}

/**
 * Main function task
 * @param void
 * @returns void
 */  
void SCON_vTask(void) {

	for (;;)
	{
		if((SCON_MSECState == MSEC_enRunning) && (SCON_ControlDomain == MSEC_enS))
		{
			/** 
			* TODO: implement body of task and define proper activation time  
			*/
		}

		OS_SLEEP((Uint32)(SCON_TaskPeriod));
	}
}




