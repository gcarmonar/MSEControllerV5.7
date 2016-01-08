/**
 * @file main_SO.h
 * @author  
 * @date  
 * @brief   
 */
#ifndef main_SO
#define main_SO

/********************************************************************/
/********************************************************************/
/********************* TERMINA AREA DE USUARIO **********************/
/********************************************************************/
/********************************************************************/


/********************************/

/**
*===============================================================
*					Module types
*===============================================================
*/

/**
 * An enumeration of TasksState
 */
typedef enum
{
	TSK_INVALID = 0,
	TSK_READY = 1,			/* task is ready to become Active */
	TSK_ACTIVE = 2,			/* task is in execution */
	TSK_INTERRUPTED = 3, 	/* task was interrupted during execution */ /* Patch JFCG */
	TSK_SLEPT = 4		 	/* task was sent to sleep */
} TskState;

/**
 * An enumeration of SortPivot
 */
typedef enum
{
	WAIT_TIME = 0,
	PRIORITY = 1
} SortingPivot;

/**
 * An StackType Struct 
 */
typedef struct
{
	Uint32 * StackTop;		/* pointer to the highest position available in the task stack */
	Uint32 * StackBottom;	/* pointer to the lowest position available in the task stack */
	Uint32 * StackPointer;	/* pointer to the current stack position, ideally between StackTop and StackBottom, grows from bottom to top */
} StackType;

/**
 * An TaskType Struct 
 */
typedef struct
{
	Uint32	Id;				/* task identifier */
	TskState			State;			/* task state */
	Uint32	WaitTime;		/* time in ms before the task become Active */
	Uint32	Priority;		/* task priority, 0 is the highest priority */
	StackType 			StackDesc;		/* task stack descriptor */

} TaskType;

/**
 * ASM MACROS
 */
#define SET_SP( ptr )						\
	pu32TempVal = ptr;						\
	asm(" MOV DP,#_pu32TempVal");			\
	asm(" MOVL ACC,@_pu32TempVal");			\
	asm(" MOV @SP,ACC")

#define GET_SP( ptr )						\
	pu32TempVal = 0;						\
	asm(" MOV ACC,SP ");					\
	asm(" MOV DP,#_pu32TempVal ");			\
	asm(" MOVL @_pu32TempVal,ACC ");		\
	ptr = (Uint32*)(0x0000FFFF & (Uint32)(pu32TempVal))

#define GET_SP_SLEEP( ptr )					\
	pu32TempVal = 0;						\
	asm(" MOV ACC,SP ");					\
	asm(" MOV DP,#_pu32TempVal ");			\
	asm(" MOVL @_pu32TempVal,ACC ");		\
	ptr = (Uint32*)(0x0000FFFF & (Uint32)(pu32TempVal))

#define RESTORE_REGISTERS_LAST_POINT()		\
	asm(" POP RPC ");						\
	asm(" MOVL XAR7, *--SP ");				\
	asm(" MOVL XAR6, *--SP ");				\
	asm(" MOVL XAR5, *--SP ");				\
	asm(" MOVL XAR4, *--SP ");				\
	asm(" MOVL XT, *--SP ");				\
	asm(" POP AR1H:AR0H ");					\
	asm(" NASP ");							\
	asm(" clrc DBGM");				/* enable debug events */\
	asm(" clrc INTM");				/* enable global interrupts */\
	asm(" IRET ")

#define PUSH_ISR_CONTEXT_INTO_STACK()	\
	asm(" PUSH RPC ");


#define RESTORE_REGISTERS_BEGINNING( ptr )		\
	pu32TempVal = (Uint32*)ptr;				\
	asm(" MOV DP,#_pu32TempVal ");				\
	asm(" MOVL ACC,@_pu32TempVal ");			\
	asm(" PUSH ACC");				/* PC */\
	asm(" PUSH ACC");				/* RPC */\
	asm(" POP RPC ");				\
	asm(" clrc DBGM");				/* enable debug events */\
	asm(" clrc INTM");				/* enable global interrupts */\
	asm(" LRETR ")

#define PUSH_CONTEXT_INTO_STACK()			\
	asm(" PUSH RPC ");						\
	asm(" PUSH AR1H:AR0H ");				\
	asm(" PUSH XAR2 ");						\
	asm(" PUSH XAR3 ");						\
	asm(" PUSH XAR4 ");						\
	asm(" PUSH XAR5 ");						\
	asm(" PUSH XAR6 ");						\
	asm(" PUSH XAR7 ");						\
	asm(" PUSH XT ");


#define POP_CONTEXT_FROM_STACK()			\
	asm(" POP XT ");						\
	asm(" POP XAR7 ");						\
	asm(" POP XAR6 ");						\
	asm(" POP XAR5 ");						\
	asm(" POP XAR4 ");						\
	asm(" POP XAR3 ");						\
	asm(" POP XAR2 ");						\
	asm(" POP AR1H:AR0H ");					\
	asm(" POP RPC ");						\
	asm(" clrc DBGM");				/* enable debug events */\
	asm(" clrc INTM");				/* enable global interrupts */\
	asm(" LRETR ");

#define PUSH_VAL_IN_STACK( val )		\
	pu32TempVal = (val);				\
	asm(" MOV DP,#_u32ValSPTemp ");		\
	asm(" MOVL ACC,@_u32ValSPTemp ");	\
	asm(" PUSH ACC ")

#define MEM_PATTERN			0xAAAAAAAA


#endif
