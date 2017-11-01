#ifndef __THREADS__
	#define __THREADS__

	#define THREAD_QUANTUM 20

	#define NULL (void *)0

	#define NUMBER_OF_PAGES 256

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


	//int threadCheck();

	//void * nextThread(void * rsp);

	threadNode * createThread(void * entryPoint);

	void * fillStackFrame(void * entryPoint, void * baseStack);

	int addThreadToProcess(int pid, void * entryPoint);

	//threadNode * createThread(void * entryPoint);

#endif