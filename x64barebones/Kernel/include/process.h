#ifndef __PROCESS
	#define __PROCESS

	// Registers are pushed into the stack instead of being stored in the pcb
	// in sysCallHandler function inside interruptions.asm

	typedef struct process {
		void * userStack;
		void * entryPoint;
		void * baseStack; // Stack base
		int pid;
	} processNode;

	typedef struct process * processPointer;

	processPointer createProcess(void * entryPoint);

	void * fillStackFrame(void * entryPoint, void * baseStack);

#endif