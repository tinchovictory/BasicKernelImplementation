#ifndef __SCHEDULER__
	#define __SCHEDULER__

	#include <process.h>

	#define QUANTUM 5

	#define NULL ((void *)0)

	typedef struct node {
		struct process * process;
		struct node * next;
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
	void addProcessToQueue(processNode * p);

	/* Create process with entryPoint */
	int addProcess(void * exec, void * entryPoint, char * name);

	/* Return the entry point of the process set as current */
	void * currentProcessEntryPoint();

	int getCurrentPid();

	int getCurrentPthread();


	/** Initialize scheduler **/
	void setKernelStack();


	void threadCheck();

	void nextThread();

	processNode * getProcessWithPid(int pid);

	threadNode * getThreadWithPthread(int pthread, processNode * process);

	int getCurrentThreadOfProcess(int pid);

	void removeProcess(int pid);

	void removeThread(int pid, int pthread);

	void removeDeadProcess();

	void removeDeadThreads(processNode * process);

	void blockProcess(int pid, processState blockType);

	void unblockProcess(int pid, processState blockType);

	void blockThread(int pid, int pthread, threadState blockType);

	void unblockThread(int pid, int pthread, threadState blockType);

	void unblockAllThreads(int pid, threadState blockType);

	int allThreadsAreBlocked(processNode * process);

	int isProcessBlocked(processState state);

	int isThreadBlocked(threadState state);

	int getParentPid(int pid);



	void yieldSwitch();

	void printAllProcess();

	void printPsHeader();

	processState getBlockProcessType(threadState state);


	/* - Debuging - */
	void printProcessStatus();
	char * getStatus(processState state);

	void printProcessPID(int pid);

#endif
