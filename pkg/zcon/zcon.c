/**
 * @file uart.c
 * @author 
 * @date 
 * @brief  
 */
 
/**
 * Z-domain Controller
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
#include "zcon.h"					// ZCon API

/**
 * Module level configuration 
 */ 
#define ZCON_N				SYS_N
#define ZCON_pfa 			SYS_a
#define ZCON_pfb 			SYS_b
#define ZCON_pfu 			SYS_u
#define ZCON_pfy 			SYS_y
#define ZCON_ControlDomain	SYS_ControlDomain
#define ZCON_TaskPeriod		SYS_SamplingPeriod
#define ZCON_MSECState		SYS_MSECState
#define ZCON_MaxVal			(float)(4.0)
#define ZCON_MinVal			(float)(-4.0)

/**
 * Module level variables 
 */  

 /**
 * Module initialization
 * @param void
 * @returns void
 */ 
void ZCON_vInit(void)
{

}

/**
 * Main function task
 * @param void
 * @returns void
 */  
void ZCON_vTask(void) {
	unsigned int i = 0;
	for (;;)
	{
		if((ZCON_MSECState == MSEC_enRunning) && (ZCON_ControlDomain == MSEC_enZ))
		{
			for(i = 0; i < ZCON_N + 1; i++)
			{
				/** 
				* y(k) = b0 u(k) + b1 u(k-1) + ... + bN u(k-N) 
				*/
				ZCON_pfy[0] += ZCON_pfb[i] * ZCON_pfu[i];
			}
			for(i = 1; i < ZCON_N + 1; i++)
			{
				/** 
				* y(k) = y(k) - a1 y(k-1) + a2 y(k-2) + ... + aN y(k-N) 
				*/
				ZCON_pfy[0] -= ZCON_pfa[i-1] * ZCON_pfy[i];
			}


		}
		else
		{

		}

		OS_SLEEP((Uint32)(ZCON_TaskPeriod));
	}
}




