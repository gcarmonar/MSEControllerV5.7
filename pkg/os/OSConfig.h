/**
 * @file OSConfig.h
 * @author  
 * @date  
 * @brief   
 */
 
#ifndef OSCONFIG_H
#define OSCONFIG_H

#include <idle.h>			/* Idle */
#include <zcon.h>			/* Z Controller */
#include <scon.h>			/* S Controller */
#include <conin.h>			/* Controller input (encoder sampling, buffer shifting) */
#include <conout.h>			/* Controller output actuator (output voltage) */
#include <comm.h>			/* Host-uC communication (state machine for Rx and Tx modes) */

#define NO_TASKS	6

#define TaskStack0_Idle_SIZE	70
#define TaskStack1_ZCon_SIZE	70
#define TaskStack2_SCon_SIZE	70
#define TaskStack3_ConIn_SIZE	70
#define TaskStack4_ConOut_SIZE	70
#define TaskStack5_Comm_SIZE	70

extern Uint32	TaskStack0_Idle[];
extern Uint32	TaskStack1_ZCon[];
extern Uint32	TaskStack2_SCon[];
extern Uint32	TaskStack3_ConIn[];
extern Uint32	TaskStack4_ConOut[];
extern Uint32	TaskStack5_Comm[];

/**
* Priority: 1 is the highest priority 
*/
#define INIT_TASKS	{					\
						{	/*** FcnIdleTask ***/\
							100,		/* Id */\
							TSK_READY,	/* State */\
							0,			/* WaitTime */\
							100,		/* Priority */\
							{			\
								&TaskStack0_Idle[TaskStack0_Idle_SIZE-1],		/* StackTop */\
								&TaskStack0_Idle[0],						/* StackBottom */\
								&TaskStack0_Idle[1]						/* StackPointer */\
							}			\
						},				\
						{	/*** FcnZConTask ***/\
							101,		/* Id */\
							TSK_READY,	/* State */\
							100,		/* WaitTime */\
							1,			/* Priority */\
							{			\
								&TaskStack1_ZCon[TaskStack1_ZCon_SIZE-1],		/* StackTop */\
								&TaskStack1_ZCon[0],						/* StackBottom */\
								&TaskStack1_ZCon[1]						/* StackPointer */\
							}			\
						},				\
						{	/*** FcnSConTask ***/\
							102,		/* Id */\
							TSK_READY,	/* State */\
							100,		/* WaitTime */\
							1,			/* Priority */\
							{			\
								&TaskStack2_SCon[TaskStack2_SCon_SIZE-1],		/* StackTop */\
								&TaskStack2_SCon[0],						/* StackBottom */\
								&TaskStack2_SCon[1]						/* StackPointer */\
							}			\
						},				\
						{	/*** FcnConInputTask ***/\
							103,		/* Id */\
							TSK_READY,	/* State */\
							50,		/* WaitTime */\
							6,			/* Priority */\
							{			\
								&TaskStack3_ConIn[TaskStack3_ConIn_SIZE-1],		/* StackTop */\
								&TaskStack3_ConIn[0],						/* StackBottom */\
								&TaskStack3_ConIn[1]						/* StackPointer */\
							}			\
						},				\
						{	/*** FcnConOutputTask ***/\
							104,		/* Id */\
							TSK_READY,	/* State */\
							150,		/* WaitTime */\
							1,			/* Priority */\
							{			\
								&TaskStack4_ConOut[TaskStack4_ConOut_SIZE-1],		/* StackTop */\
								&TaskStack4_ConOut[0],						/* StackBottom */\
								&TaskStack4_ConOut[1]						/* StackPointer */\
							}			\
						},				\
						{	/*** FcnCommTask ***/\
							105,		/* Id */\
							TSK_READY,	/* State */\
							10,			/* WaitTime */\
							10,			/* Priority */\
							{			\
								&TaskStack5_Comm[TaskStack5_Comm_SIZE-1],		/* StackTop */\
								&TaskStack5_Comm[0],						/* StackBottom */\
								&TaskStack5_Comm[1]						/* StackPointer */\
							}			\
						},				\
					}



/**
*===============================================================
*					Function prototypes
*===============================================================
*/
void InitTaskStacks(void);
void InitBSP(void);

#endif
