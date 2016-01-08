/**
 *  \brief     Preemptive OS Kernel.
 *  \details   OS-Kernel based on the pre-emptive kernel provided in the Embedded Systems course.
 *  \author    Jalil Chavez
 *  \author    Jose Pesado
 *  \version   4.5
 *  \date      2015
 *  \pre       Configure the tasks in application file MSECConfig.h.
 *  \warning   Tested and released for TI C2000 F28335.
 *  \copyright GNU Public License.
 */

#include <DSP2833x_Device.h>     /**< DSP2833x Headerfile Include File */
#include <DSP2833x_GlobalPrototypes.h>         // Prototypes for global functions within the
                                              // .c files.
#include "main_SO.h"
#include "OSConfig.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

/***** PROTOTIPOS DE LAS FUNCIONES DEL SISTEMA OPERATIVO *****/
void InitSO(void);		//Inicializa SO (Limpia tablas y pone hilos en tabla READY para que inicien activos)
void OS_SLEEP(Uint32);			//Asigna el tiempo que duerme la tarea / Se comparte externamente
void OS_SCHEDULER(void);		//Organiza en prioridades las tabla READY
void InitStack(StackType Stack);
/* Context handling functions */
void SaveContextFromISR(StackType * Stack);
void RestoreInterruptedTaskContext(StackType Stack);
void SaveContextFromSleep(StackType * Stack);
void RestoreSleptTaskContext(StackType Stack);
/* Auxiliary functions */
void SetAlarm(unsigned long int WaitTime);
unsigned long int ElapsedTimeSinceLastScheduling(void);
int Sort(TaskType * TaskList, unsigned int ListSize, SortingPivot Pivot);
interrupt void cpu_timer0_isr(void);
void CheckTaskOverflow(void);
void InitGpioLED(void);
/*************************************************************/

static Uint32 * pu32TempVal;

unsigned long int	*PtrStack_Global;	/** El Ptr stack del stack del SO */

TaskType	ReadyTasks[NO_TASKS] = INIT_TASKS;	/** collection of tasks ready to be executed */
TaskType	ActiveTask;				/** task currently in execution */
unsigned long int NextSchedulingDelay;

/**
 * Añadir descripcion de la funcion
 * @param void
 * @returns void
 */ 
void main(void)
{
	InitSysCtrl();
	DINT;
	InitGpioLED();
	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

	InitSO();

	/***** GUARDAMOS SP GLOBAL *****/
	asm(" MOV ACC,@SP ");
	asm(" MOV DP, #_PtrStack_Global");
	asm(" MOVL @_PtrStack_Global,ACC ");

	/***** ECHAMOS A CORRER EL SISTEMA OPERATIVO *****/
	OS_SCHEDULER();
}

/**
 * Añadir descripcion de la funcion
 * @param void
 * @returns void
 */ 
void InitSO(void)
{
	unsigned int i = 0;

	NextSchedulingDelay = 0;

	/** 
	* invalidate ActiveTask descriptor 
	*/
	ActiveTask.Id = 0;
	ActiveTask.Priority = 0;
	ActiveTask.State = TSK_INVALID;
	ActiveTask.WaitTime = 0;
	ActiveTask.StackDesc.StackBottom = 0;
	ActiveTask.StackDesc.StackTop = 0;
	ActiveTask.StackDesc.StackPointer = 0;

	for(i = 0; i < NO_TASKS; i++)
	{
		InitStack(ReadyTasks[i].StackDesc);
	}

	InitTaskStacks();

	InitBSP();
}

#pragma CODE_SECTION(OS_SCHEDULER, "funsec")
/**
 * Añadir descripcion de la funcion
 * @param void
 * @returns void
 */ 
void OS_SCHEDULER(void)
{
	/** 
	* disable global interrupts, the scheduler routine is non-preemptive 
	*/
	DINT;
	DRTM;

	/** 
	* Restore context of scheduler, the global stack 
	*/
	asm(" MOV DP,#_PtrStack_Global");
	asm(" MOVL ACC,@_PtrStack_Global");
	asm(" MOV SP,ACC");

	unsigned int i;
	unsigned int j;
	unsigned long int WaitTime;
	unsigned int GroupSize;
	TaskType NextTask;

	/** 
	* sort Ready tasks by WaitTime 
	*/
	(void)Sort(ReadyTasks, NO_TASKS, WAIT_TIME);
	/** 
	* Check integrity of task stacks 
	*/
	CheckTaskOverflow();
	/** 
	*find and sort by Priority group of tasks of same WaitTime 
	*/
	for(i=0; i<NO_TASKS; )
	{
		GroupSize = 1;
		WaitTime = ReadyTasks[i].WaitTime;
		/** search for tasks with same WaitTime */
		for(j=i+1; j<NO_TASKS; j++)
		{
			if(ReadyTasks[j].WaitTime>WaitTime)
			{
				/** 
				* another group starts at j-th task, break the search 
				*/
				break;
			}
			else
			{
				/** 
				* j-th task is from the same group as task i 
				*/
				GroupSize++;
			}
		}
		if(GroupSize>1)
		{
			/** 
			* sort group of tasks by Priority 
			*/
			(void)Sort(&ReadyTasks[i], GroupSize, PRIORITY);
		}
		/** 
		* find next group of tasks 
		*/
		i = j;
	}

	/** 
	* the first task in the list is the shortest wait time and higher priority 
	*/
	if(ReadyTasks[0].WaitTime == 0)
	{
		/** 
		* it's time to activate task 
		*/
		NextTask.State = TSK_INVALID;
		/** 
		* find the next higher priority task (0 is the highest priority) 
		*/
		for(i=1; i<NO_TASKS; i++)
		{
			if(ReadyTasks[i].Priority < ReadyTasks[0].Priority)
			{
				/** 
				* the scheduler shall be executed again in ReadyTasks[i].WaitTime 
				*/
				NextTask = ReadyTasks[i];
				break;
			}
		}
		if(NextTask.State == TSK_INVALID)
		{
			/** 
			* there is no task with higher priority than the task about to be activated  
			* the scheduler will be called whenever the coming task finishes */
		}
		else
		{
			/** 
			* the scheduler shall be called again in NextTask.WaitTime 
			*/
			SetAlarm(NextTask.WaitTime);
		}

		ActiveTask = ReadyTasks[0];

		/** 
		* Remove task to be activated from the ReadyTasks list 
		*/
		for(i=1; i<NO_TASKS; i++)
		{
			ReadyTasks[i-1] = ReadyTasks[i];
		}
		ReadyTasks[i-1].Id = 0;
		ReadyTasks[i-1].Priority = 0;
		ReadyTasks[i-1].State = TSK_INVALID;
		ReadyTasks[i-1].WaitTime = 0;
		ReadyTasks[i-1].StackDesc.StackBottom = 0;
		ReadyTasks[i-1].StackDesc.StackTop = 0;
		ReadyTasks[i-1].StackDesc.StackPointer = 0;

		/** 
		* Restore context of task to be executed (interrupts are enabled just before jumping to the interrupted task) 
		*/
		switch( ActiveTask.State )
		{
			/** 
			* Scheduler was called from OS_SLEEP function 
			*/
			case TSK_SLEPT :
				ActiveTask.State = TSK_ACTIVE;
				RestoreSleptTaskContext(ActiveTask.StackDesc);
			break;
			/** 
			* Scheduler was called from interrupt routine 
			*/
			case TSK_INTERRUPTED :
				ActiveTask.State = TSK_ACTIVE;
				RestoreInterruptedTaskContext(ActiveTask.StackDesc);
			break;
			/** 
			* Task is ready to run 
			*/
			case TSK_READY :
				ActiveTask.State = TSK_ACTIVE;
				SET_SP(ActiveTask.StackDesc.StackPointer);
				RESTORE_REGISTERS_BEGINNING(ActiveTask.StackDesc.StackBottom[0]);
			break;
			/** 
			* Invalid caller 
			*/
			default :

			break;
		}
	}
	else
	{
		/** 
		* set a timer to delay activation of the task 
		*/
		SetAlarm(ReadyTasks[0].WaitTime);

		/** 
		* now the interrupts are enabled 
		*/
		EINT;
		ERTM;
	}

	for(;;);
}

/**
 * Añadir descripcion de la funcion
 * @param Uint32 HiloSleepTime
 * @returns void
 */ 
void OS_SLEEP(Uint32 HiloSleepTime) {
	CpuTimer0Regs.TCR.bit.TSS = 0;
	DINT;
	DRTM;
	ActiveTask.WaitTime = HiloSleepTime;

	/** 
	* we are not interested in keeping in the stack the argument to the jump to OS_SLEEP 
	*/
	asm(" SUBB SP,#2 ");

	/** 
	* ActiveTask was self-preempted  
	* context is saved into the current task Stack starting at Stack.StackPointer 
	* the stack increase in direction of Stack.StackTop but never beyond */
	PUSH_CONTEXT_INTO_STACK();
	GET_SP_SLEEP(ActiveTask.StackDesc.StackPointer);
	//SaveContextFromSleep(&ActiveTask.StackDesc);


	/** 
	* Notify that ActiveTask was sent to sleep 
	*/
	ActiveTask.State = TSK_SLEPT;
	/** 
	* move ActiveTask to the ReadyTask list 
	*/
	ReadyTasks[NO_TASKS-1] = ActiveTask;
	/** 
	* Invalidate task 
	*/
	ActiveTask.State = TSK_INVALID;
	/** 
	* jump to the scheduler 
	*/

	/** 
	* SP = RPC  
	* SP has now the address of the caller of OS_SLEEP, however we don't care about this address since we will load SP with the address of the system stack 
	* RPC = PC + 2  
	* RPC is loaded with the address of OS_SLEEP, but we are not interested to return to this point 
	* PC = _OS_SCHEDULER 
	* _OS_SCHEDULER is the new value of PC 
	*/
	asm(" LCR _OS_SCHEDULER ");
}

/**
 * Añadir descripcion de la funcion
 * @param StackType Stack
 * @returns void
 */ 
void InitStack(StackType Stack)
{
	memset(Stack.StackBottom,(int)MEM_PATTERN,(Stack.StackTop-Stack.StackBottom + 1)*2);
}

/**
 * Añadir descripcion de la funcion
 * @param StackType * Stack
 * @returns void
 */ 
void SaveContextFromISR(StackType * Stack)
{
	/** 
	* current context is saved in Stack starting at Stack.StackPointer 
	* the stack grows in direction of Stack.StackTop but never beyond 
	*/
	GET_SP(ActiveTask.StackDesc.StackPointer);
}

/**
 * Añadir descripcion de la funcion
 * @param StackType Stack
 * @returns void
 */ 
void RestoreInterruptedTaskContext(StackType Stack)
{
	/** 
	* context is restored from Stack.StackPointer  
	* IRET instruction is used in order to take advantage of the
	* context stored at interruption point
	*/
	SET_SP(ActiveTask.StackDesc.StackPointer);
	RESTORE_REGISTERS_LAST_POINT();
}

/**
 * Añadir descripcion de la funcion
 * @param StackType * Stack
 * @returns void
 */ 
void SaveContextFromSleep(StackType * Stack)
{
	/** 
	* current context is saved in Stack starting at Stack.StackPointer 
	* the stack grows in direction of Stack.StackTop but never beyond 
	* as interrupt did not ocurr, it is necessary to save context
	* manually 
	*/
	PUSH_CONTEXT_INTO_STACK();
	GET_SP(ActiveTask.StackDesc.StackPointer);
}

/**
 * Añadir descripcion de la funcion
 * @param StackType Stack
 * @returns void
 */ 
void RestoreSleptTaskContext(StackType Stack)
{
	/** 
	* context is restored from Stack starting at Stack.StackPointer  
	* the stack decrease in direction of Stack.StackBottom but never below 
	*/
	SET_SP(ActiveTask.StackDesc.StackPointer);
	POP_CONTEXT_FROM_STACK();
}

/**
 * Añadir descripcion de la funcion
 * @param unsigned long int WaitTime
 * @returns void
 */
void SetAlarm(unsigned long int WaitTime)
{
	NextSchedulingDelay = WaitTime;

	EALLOW;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	EDIS;

	CpuTimer0Regs.TCR.bit.TSS = 1;
	InitCpuTimers();
	ConfigCpuTimer(&CpuTimer0, 150, (float)(WaitTime*1000));
	CpuTimer0Regs.TCR.all = 0x4001; /** Use write-only instruction to set TSS bit = 0 */
	IER |= M_INT1;
	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
}

/**
 * Añadir descripcion de la funcion
 * @param void
 * @returns unsigned long int
 */
unsigned long int ElapsedTimeSinceLastScheduling(void)
{
	return NextSchedulingDelay;
}

/**
 * Añadir descripcion de la funcion
 * @param TaskType * TaskList, unsigned int ListSize, SortingPivot Pivot
 * @returns int
 */
int Sort(TaskType * TaskList, unsigned int ListSize, SortingPivot Pivot)
{
	TaskType TaskBkp;
	unsigned long int PivotValueCurrent;
	unsigned long int PivotValueNext;
	unsigned int i;
	unsigned int j;

	/** 
	* Bubble sort 
	*/
	for(j=0; j<ListSize-1; j++)
	{
		for(i=0; i<ListSize-1; i++)
		{
			switch(Pivot)
			{
				case WAIT_TIME:
					PivotValueCurrent = TaskList[i].WaitTime;
					PivotValueNext = TaskList[i+1].WaitTime;
					break;
				case PRIORITY:
					PivotValueCurrent = TaskList[i].Priority;
					PivotValueNext = TaskList[i+1].Priority;
					break;
				default:
					return -1;
			}

			if(PivotValueCurrent > PivotValueNext)
			{
				/** 
				* swap position of Current and Next 
				*/
				TaskBkp = TaskList[i+1];
				TaskList[i+1] = TaskList[i];
				TaskList[i] = TaskBkp;
			}
		}
	}
	return 0;
}

/**
 * Añadir descripcion de la funcion
 * @param void
 * @returns void
 */
void UpdateReadyTaskTime( void )
{
	unsigned long int ElapsedTime;
	unsigned int i = 0;
	/** 
	* Update the wait time of all the ready tasks 
	*/
	ElapsedTime = ElapsedTimeSinceLastScheduling();
	for(i=0;i<NO_TASKS;i++)
	{
		if(ReadyTasks[i].WaitTime >= ElapsedTime)
		{
			ReadyTasks[i].WaitTime -= ElapsedTime;
		}
		else
		{
			ReadyTasks[i].WaitTime = 0;
		}
	}
}

/**
 * Añadir descripcion de la interrupcion
 * @param void
 * @returns void
 */
interrupt void cpu_timer0_isr(void)
{
	DINT;
	DRTM;
	CpuTimer0.InterruptCount++;
	/** 
	* Acknowledge this interrupt to receive more interrupts from group 1
	*/
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
	if( ActiveTask.State != TSK_INVALID )
	{
		/** 
		* save context of task in execution 
		*/
		PUSH_ISR_CONTEXT_INTO_STACK();
		GET_SP(ActiveTask.StackDesc.StackPointer);
		/** 
		* Notify that ActiveTask was interrupted 
		*/
		ActiveTask.State = TSK_INTERRUPTED;
		/** 
		* move ActiveTask to the ReadyTask list 
		*/
		ReadyTasks[NO_TASKS-1] = ActiveTask;
	}
	else
	{

	}
	UpdateReadyTaskTime();
	NextSchedulingDelay = 0;
	/* SP = RPC */
	/* RPC = PC + 2 */
	/* PC = Destiny */
	asm(" LCR _OS_SCHEDULER ");
}

/**
 * Añadir descripcion de la funcion
 * @param void
 * @returns void
 */
void InitGpioLED(void)
{
    EALLOW;

    /** 
	* Paso 0: Por seguridad, se inicializan todos los puertos como GPIO 
	*/
	GpioCtrlRegs.GPAMUX1.all = 0x00000000;  /** All GPIO */
	GpioCtrlRegs.GPAMUX2.all = 0x00000000;  /** All GPIO */


	/** 
	* Paso 2: Habilitar/Deshabilitar Pull-up 
	*/
	GpioCtrlRegs.GPAPUD.bit.GPIO31 = 1;		/** LED 1 */
	GpioCtrlRegs.GPBPUD.bit.GPIO34 = 1;		/** LED 2 */

	/** 
	* Paso 3: Seleccionar Cualificacion de la entrada 
	* Para usar default (SYSCLKOUT) lo dejamos sin modificar 
	*/
	GpioCtrlRegs.GPACTRL.all = 0x0000000;

	/** 
	* Paso 4: Seleccionar funcion del puerto  
	* En el paso 0 se establecieron todos los puertos como GPIO, modificar solo en caso de tener otra funcion 
	* Paso 5: Sentido del puerto (1=Out, 0=In) 
	*/
	GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;
	GpioDataRegs.GPBSET.bit.GPIO34 = 1;

	GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;		/** Los GPIO 31 y 34 los podemos visualizar en la Control Card */
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;

	/** 
	* Paso 6: Seleccionar si el puerto despierta al CPU de HALT o del modo de bajo consumo
	* Si no lo hace despertar se omite  
	* Paso 7: Seleccionar si el puerto puerto estara configurado como interrupcion externa
	* Si no se configurara como interrupcion se omite 
	*/

  	EDIS;

    //return;
} //Fin IniciaGpio

/**
 * Añadir descripcion de la funcion
 * @param void
 * @returns void
 */
void CheckTaskOverflow(void)
{
	Uint32 u32TskIndex = 0;
	Uint32 u32StackTopMark = 0;

	for(u32TskIndex = 0; u32TskIndex < NO_TASKS; u32TskIndex++)
	{
		u32StackTopMark = *(ReadyTasks[u32TskIndex].StackDesc.StackTop);
		if( u32StackTopMark != MEM_PATTERN )
		{
			/** 
			* Stop program here and wait for a watchdog reset 
			*/
			for(;;);
		}
		else
		{
			/** 
			* Stack is within allowed limits 
			*/
		}
	}
}
/****** Fin del Sistema Operativo **********/
