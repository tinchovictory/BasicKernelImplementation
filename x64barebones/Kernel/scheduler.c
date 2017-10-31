#include <naiveConsole.h>
#include "systemCalls.h"
#include <scheduler.h>
#include <process.h>

#include <memory.h> //TESING THEN REPLACE WITH MEMORY ALLOCATOR
#include <naiveConsole.h>

#define NULL 0

// Ref.: https://gitlab.com/RowDaBoat/Wyrm/blob/master/Kernel/Scheduler/Scheduler.cpp

static void * kernelStack; // Pasar a .c correspondiente. Ver tipo y posición de memoria para declaración.

static schedulerQueue processQueue = NULL;
static schedulerQueue currentProcess = NULL;
static int queueSize = 0;

static int numberOfTicks = 0;

void * switchUserToKernel(void * esp) {
	processPointer process = currentProcess->process; // Pointer to process that I am about to deallocate.
	process->userStack = esp; // Save stack pointer.
	return kernelStack;
}

void runScheduler() {
	if(currentProcess == NULL) {
		return;
	}

	//Check quantum
	if(numberOfTicks < QUANTUM) {
		numberOfTicks++;
		return;
	}
	numberOfTicks = 0;
	ncNewline();
	ncPrint("Switch");
	ncNewline();
	currentProcess = currentProcess->next;
}

void * switchKernelToUser () {
	processPointer process = currentProcess->process;
	return process->userStack;
}

int addProcess(void * entryPoint) {
	processPointer process = createProcess(entryPoint);
	addProcessToQueue(process);
	return process->pid;
}

void addProcessToQueue(processPointer p) {
	schedulerNode * node = (schedulerNode *) malloc(1000);

	node->process = p;

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
	return currentProcess->process->entryPoint;
}

void setKernelStack() {
	kernelStack = (void *) malloc(1000);
}
