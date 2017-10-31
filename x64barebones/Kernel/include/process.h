#ifndef __PROCESS
	#define __PROCESS

	#define THREAD_QUANTUM 20

	/*
		Registers are pushed into the stack instead of being stored in the pcb
		in processSwitch function inside schedulerSwitch.asm
	*/

	typedef struct thread {
		void * userStack;
		void * entryPoint;
		void * baseStack;
		int pthread;
	} threadNode;

	typedef struct libNode {
		struct thread * thread;
		struct libNode * next;
	} threadLibrary;

	typedef struct process {
		void * userStack; // Stack pointer
		void * entryPoint;
		void * baseStack; // Stack base
		int pid;

		threadLibrary * threadLibrary;
		threadLibrary * currentThread;
	} processNode;

	typedef struct process * processPointer;

	/* Creates a process in memory from the process entry point */
	processPointer createProcess(void * entryPoint);

	/* Create a stack frame for proces initialization */
	void * fillStackFrame(void * entryPoint, void * baseStack);

#endif