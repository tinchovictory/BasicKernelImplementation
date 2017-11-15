#ifndef __THREADS__
	#define __THREADS__

	#define THREAD_QUANTUM 1

	#define NULL ((void *)0)

	#define NUMBER_OF_PAGES 256

	typedef enum {T_READY, T_RUNNING, T_BLOCKED_IO, T_BLOCKED_MUTEX, T_BLOCKED_PIPE, T_DEAD} threadState;

	typedef struct thread {
		void * userStack;
		void * entryPoint;
		void * baseStack;
		int pthread;
		threadState state;
	} threadNode;

	typedef struct libNode {
		struct thread * thread;
		struct libNode * next;
	} threadLibrary;

	threadNode * createThread(void * exec, void * entryPoint, int currentPThread);

	void * fillStackFrame(void * exec, void * entryPoint, void * baseStack);

	int addThreadToProcess(int pid, void * exec, void * entryPoint);

	void freeThreadLibrary(threadLibrary * library, int threadSize);

#endif