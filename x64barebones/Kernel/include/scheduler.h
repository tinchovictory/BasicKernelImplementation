#ifndef __SCHEDULER
	#define __SCHEDULER

	#include <process.h>

	#define QUANTUM 100

	typedef struct node {
		struct process * process;
		struct node * next;
	} schedulerNode;

	typedef struct node * schedulerQueue;

	void * switchUserToKernel(void * esp);

	void runScheduler();

	void * switchKernelToUser();


	void addProcessToQueue(processPointer p);

	int addProcess(void * entryPoint);

	void * currentProcessEntryPoint();

	void setKernelStack();

#endif
