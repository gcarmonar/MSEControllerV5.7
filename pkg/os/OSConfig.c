/**
 * @file OSConfig.c
 * @author  
 * @date  
 * @brief   
 */
 
/**
* Foreign header files
*/ 
#include <DSP2833x_Device.h>     // DSP2833x Headerfile Include File

/**
 * Internal header files
 */
#include "OSConfig.h"

/**
 * OS configuration
 */ 

 /**
 * Local Variables
 **/
Uint32	TaskStack0_Idle [TaskStack0_Idle_SIZE];
Uint32	TaskStack1_ZCon [TaskStack1_ZCon_SIZE];
Uint32	TaskStack2_SCon [TaskStack2_SCon_SIZE];
Uint32	TaskStack3_ConIn [TaskStack3_ConIn_SIZE];
Uint32	TaskStack4_ConOut [TaskStack4_ConOut_SIZE];
Uint32	TaskStack5_Comm [TaskStack5_Comm_SIZE];

/**
 * Añadir descripcion de la funcion
 * @param void
 * @returns void
 */ 
 void InitTaskStacks(void)
{
	TaskStack0_Idle[0] = (Uint32)&IDLE_vTask;			/** Idle task */
	TaskStack1_ZCon[0] = (Uint32)&ZCON_vTask;			/** Z Controller task */
	TaskStack2_SCon[0] = (Uint32)&SCON_vTask;			/** S Controller task */
	TaskStack3_ConIn[0] = (Uint32)&CONIN_vTask;		/** Controller input sampling task (encoder sampling, buffer shifting) */
	TaskStack4_ConOut[0] = (Uint32)&CONOUT_vTask;		/** Controller output actuator task (output voltage) */
	TaskStack5_Comm[0] = (Uint32)&COMM_vTask;			/** Host-uC communication task (state machine for Rx and Tx modes) */
}

/**
 * Añadir descripcion de la funcion
 * @param void
 * @returns void
 */ 
 void InitBSP(void)
{
	IDLE_vInit();
	ZCON_vInit();
	SCON_vInit();
	CONIN_vInit();
	CONOUT_vInit();
	COMM_vInit();
}
