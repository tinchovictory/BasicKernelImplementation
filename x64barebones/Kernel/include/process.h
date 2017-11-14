#ifndef __PROCESS
	#define __PROCESS

	#include <threads.h>

	#define NULL ((void *)0)

	#define NUMBER_OF_PAGES 256

	typedef enum {READY, RUNNING, BLOCKED, DEAD} processState;

	/*
		Registers are pushed into the stack instead of being stored in the pcb
		in processSwitch function inside schedulerSwitch.asm
	*/

	
	typedef struct process {
		int pid;
		processState state;
		char * name;//cambiar

		threadLibrary * threadLibrary;
		threadLibrary * currentThread;
		int threadSize;

		int threadTick;

		int currentPThread;
	} processNode;

	typedef struct process * processPointer;

	/* Creates a process in memory from the process entry point */
	processPointer createProcess();

	/* Create a stack frame for proces initialization */
	void * fillStackFrame(void * entryPoint, void * baseStack);

#endif