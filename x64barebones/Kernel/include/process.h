#ifndef __PROCESS
	#define __PROCESS

	// Registers are pushed into the stack instead of being stored in the pcb
	// in sysCallHandler function inside interruptions.asm

	typedef struct process {
		void * userStack;
		void * entryPoint;
		void * baseStack; // Stack base
		void * limitStack;// Stack limit
		int remainingTime; // time left to terminate
	} processNode;

	typedef struct process * processPointer;

#endif