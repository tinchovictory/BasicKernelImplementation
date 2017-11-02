#ifndef __SCHEDULER
	#define __SCHEDULER

	#include <process.h>

	#define QUANTUM 100

	#define NULL ((void *)0)

	#define NO_SWITCH 0
	#define PROCESS_SWITCH 1
	#define THREAD_SWITCH 2


	typedef struct node {
		struct process * process;
		struct node * next;
		int threadTick;
	} schedulerNode;

	typedef struct node * schedulerQueue;

	/** Functions for process switch **/

	/* Switch to kernel mode, save process stack pointer. Returns kernel stack pointer */
	void * switchUserToKernel(void * esp);

	/* Process switch after QUANTUM ticks */
	void runScheduler();

	/* Switch to user mode. Returns user stack pointer of current process */
	void * switchKernelToUser();


	/** Functions for loading a new process **/

	/* Add process p to process queue */
	void addProcessToQueue(processPointer p);

	/* Create process with entryPoint */
	int addProcess(void * entryPoint);

	/* Return the entry point of the process set as current */
	void * currentProcessEntryPoint();


	/** Initialize scheduler **/
	void setKernelStack();


	int threadCheck();

	void * nextThread(void * rsp);

	processNode * getProcessWithPid(int pid);

#endif
