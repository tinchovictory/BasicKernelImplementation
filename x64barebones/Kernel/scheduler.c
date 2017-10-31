#include <naiveConsole.h>
#include "systemCalls.h"
#include <scheduler.h>
#include <process.h>

#include <memoryManager.h> //TESING THEN REPLACE WITH MEMORY ALLOCATOR
#include <naiveConsole.h>

#define NULL 0

// Ref.: https://gitlab.com/RowDaBoat/Wyrm/blob/master/Kernel/Scheduler/Scheduler.cpp

static void * kernelStack; // Pasar a .c correspondiente. Ver tipo y posición de memoria para declaración.

static schedulerQueue processQueue = NULL;
static schedulerQueue currentProcess = NULL;
static int queueSize = 0;

static int numberOfTicks = 0;

void * switchUserToKernel(void * esp) {
	//processPointer process = currentProcess->process; // Pointer to process that I am about to deallocate.
	//process->userStack = esp; // Save stack pointer.
	currentProcess->process->userStack = esp; // sin esto no anda
//	ncPrint("Switch user to kernel -> userStack: ");ncPrintHex(esp);ncNewline();
	//ncPrint("process->userStack: ");ncPrintHex(process->userStack);ncNewline();
	//ncPrint("currentProcess->process->userStack: ");ncPrintHex(currentProcess->process->userStack);ncNewline();
//	ncPrint("Switch user to kernel -> kernelStack: ");ncPrintHex(kernelStack);ncNewline();
	return kernelStack;
}

void runScheduler() {
	if(currentProcess == NULL) {
		return;
	}

//	ncPrint("Im in process: ");ncPrintDec(currentProcess->process->pid);ncNewline();
	//ncPrint("process->userStack: ");ncPrintHex(currentProcess->process->userStack);ncNewline();

	//Check quantum
	if(numberOfTicks < QUANTUM) {
		numberOfTicks++;
		return;
	}
	numberOfTicks = 0;
	currentProcess = currentProcess->next;
}

void * switchKernelToUser () {
	processPointer process = currentProcess->process;
//	ncPrint("Switch kernel to User -> userStack: ");ncPrintHex(process->userStack);ncNewline();
	return process->userStack;
}

int addProcess(void * entryPoint) {
	processPointer process = createProcess(entryPoint);
	addProcessToQueue(process);
	return process->pid;
}

void addProcessToQueue(processPointer p) {
	schedulerNode * node = (schedulerNode *) allocate(0x1000);

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
	kernelStack = (void *) allocate(0x1000);
}
