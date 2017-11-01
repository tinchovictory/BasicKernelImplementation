#include <threads.h>
#include <stdint.h>
#include <stackFrame.h>
#include <memoryManager.h>
#include <process.h>
#include <scheduler.h>


int addThreadToProcess(int pid, void * entryPoint) {
	threadNode * thread = createThread(entryPoint);

	threadLibrary * node = (threadLibrary *) allocate(PAGE_SIZE);

	node->thread = thread;

	processNode * process = getProcessWithPid(pid);

	if(process == NULL) {
		return -1;
	}


	if(process->currentThread == NULL) {
		process->threadLibrary = node;
		process->currentThread = node;
		process->threadLibrary->next = process->threadLibrary;
	} else {
		node->next = process->threadLibrary->next;
		process->threadLibrary->next = node;
	}

	//queueSize++;

	return thread->pthread;
}

static int currentPthread = 0; // TIENE QUE ESTAR EN PROCESS


threadNode * createThread(void * entryPoint) {
	/* Create process in memory, asign a base pointer, initialize stack frame */
	threadNode * thread = (threadNode *) allocate(PAGE_SIZE);
	thread->entryPoint = entryPoint;
	thread->baseStack = (void *) allocate(PAGE_SIZE * NUMBER_OF_PAGES);
	thread->userStack = fillStackFrame(entryPoint, thread->baseStack);
	thread->pthread = currentPthread;

	currentPthread++;

	return thread;
}