#include <naiveConsole.h>
#include "systemCalls.h"
#include <scheduler.h>
#include <process.h>
#include <threads.h>

#include <memoryManager.h> //TESING THEN REPLACE WITH MEMORY ALLOCATOR
#include <naiveConsole.h>

// Ref.: https://gitlab.com/RowDaBoat/Wyrm/blob/master/Kernel/Scheduler/Scheduler.cpp

static void * kernelStack; // Pasar a .c correspondiente. Ver tipo y posición de memoria para declaración.

static schedulerQueue processQueue = NULL;
static schedulerQueue currentProcess = NULL;
static int queueSize = 0;

static int numberOfTicks = 0;

void * switchUserToKernel(void * esp) {
	currentProcess->process->currentThread->thread->userStack = esp;
	return kernelStack;
}

void runScheduler() {
	if(currentProcess == NULL) {
		return;
	}

	/* Check quantum */
	if(numberOfTicks < QUANTUM) {
		numberOfTicks++;
		/* Check thread quantum */
		threadCheck();
		return;
	}

	/* Run next process */
	numberOfTicks = 0;
	currentProcess = currentProcess->next;
}

void * switchKernelToUser () {
	processPointer process = currentProcess->process;
	return process->currentThread->thread->userStack;
	
}

int addProcess(void * entryPoint) {
	processPointer process = createProcess();
	addProcessToQueue(process);
	addThreadToProcess(process->pid, entryPoint);
	return process->pid;
}

void addProcessToQueue(processPointer p) {
	schedulerNode * node = (schedulerNode *) allocate(PAGE_SIZE);

	node->process = p;
	node->threadTick = 0;

	if(currentProcess == NULL) {
		processQueue = node;
		currentProcess = node;
		processQueue->next = processQueue;
	} else {
		node->next = processQueue->next;
		processQueue->next = node;
	}

	queueSize++;
}

void * currentProcessEntryPoint() {
	return currentProcess->process->currentThread->thread->entryPoint;
}

void setKernelStack() {
	kernelStack = (void *) allocate(PAGE_SIZE);
}


processNode * getProcessWithPid(int pid) {
	schedulerQueue aux = processQueue->next;

	while(aux != processQueue && aux->process->pid != pid) {
		aux = aux->next;
	}

	if(aux->process->pid == pid) {
		return aux->process;
	}

	return NULL;
}

/* Threads */

void threadCheck() {

	if( (currentProcess->threadTick) < THREAD_QUANTUM) {
		currentProcess->threadTick++;
		return;
	}
	currentProcess->threadTick = 0;
	nextThread();
}

void nextThread() {
	currentProcess->process->currentThread = currentProcess->process->currentThread->next;
}