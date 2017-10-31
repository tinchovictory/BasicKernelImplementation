#ifndef __PROCESS
	#define __PROCESS

	/*
		Registers are pushed into the stack instead of being stored in the pcb
		in processSwitch function inside schedulerSwitch.asm
	*/

	typedef struct process {
		void * userStack; // Stack pointer
		void * entryPoint;
		void * baseStack; // Stack base
		int pid;
	} processNode;

	typedef struct process * processPointer;

	/* Creates a process in memory from the process entry point */
	processPointer createProcess(void * entryPoint);

	/* Create a stack frame for proces initialization */
	void * fillStackFrame(void * entryPoint, void * baseStack);

#endif