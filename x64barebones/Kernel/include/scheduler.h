#ifndef __SCHEDULER
	#define __SCHEDULER

	#include <process.h>

	#define QUANTUM 10000

	typedef struct node {
		struct process * content;
		struct node * next;
	} schedulerNode;

	int quantumCheck();

	void * switchUserToKernel(void * esp);

	void * switchKernelToUser(void * esp);

	typedef struct node * schedulerQueue;

	// Add process to tail of scheduler circular queue.
	int offer(void * process);

	// Scheduler moves to the next process to serve.
	int advance();

	int processTime();

	int freeMemory();

	int setQuantum();

	/* Returns 1 if the queue is empty */
	int isEmpty();

	/* Returns the queue size */
	int size();

	/* See schedulerSwitch.asm */
	int processSwitch();

#endif
