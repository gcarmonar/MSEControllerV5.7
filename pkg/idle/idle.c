/**
 * @file idle.c
 * @author  
 * @date  
 * @brief  
 *
 */
 /**
 * Idle
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

#include "idle.h"				// Idle API

/**
 * Module level configuration 
 */


/**
 * Module level variables 
 */

Uint32 IDLE__u32LoopCounter;

/**
 * Local functions
 */
void IDLE_vDummy(Uint32 u32Loop);

/**
 * Module initialization
 * @param void
 * @returns void
 */  
void IDLE_vInit(void)
{
	IDLE__u32LoopCounter = 0;
}

/**
 * Main function task
 * @param void
 * @returns void
 */  
void IDLE_vTask(void) {
	Uint32 i = 0;
	for (;;)
	{
		IDLE_vDummy(i++);
	}
}

#pragma CODE_SECTION(IDLE_vDummy, "funsec")
/**
 * Añadir descripcion de la funcion
 * @param Uint32 u32Loop
 * @returns void
 */  
void IDLE_vDummy(Uint32 u32Loop)
{
	Uint16 u16Dummy = 0;
	Uint16 u16Counter = 1;

	for(u16Counter=1; u16Counter < u32Loop; u16Counter++)
	{
		if(u16Counter == 1)
		{
			IDLE__u32LoopCounter++;
		}
		u16Dummy += IDLE__u32LoopCounter * u16Counter;
	}
}




