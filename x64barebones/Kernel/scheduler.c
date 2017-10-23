#include "systemCalls.h"

#include <scheduler.h>

// Ref.: https://gitlab.com/RowDaBoat/Wyrm/blob/master/Kernel/Scheduler/Scheduler.cpp

static void * kernelStack; // Pasar a .c correspondiente. Ver tipo y posición de memoria para declaración.

static schedulerQueue queue;

static schedulerQueue tail;

static int counter = 0;

static int quantum = QUANTUM;

static int queueSize = 0;

int quantumCheck () {
	counter++;

	if (counter >= QUANTUM) {
		counter = 0;
		processSwitch(); // inside schedulerSwitch.asm
		return 1;
	}
	return 0;
}

void * switchUserToKernel(void * esp) {
	processPointer process = queue->content; // Pointer to process that I am about to deallocate.
	process->userStack = esp; // Save stack pointer.
	return kernelStack;
}

void * switchKernelToUser (void * esp) {
	advance();
	processPointer process = queue->content;
	kernelStack = esp; // Save kernel stack pointer.
	return process->userStack;
}

// Add process to tail of scheduler circular queue.
int offer(void * process) {
	schedulerQueue auxN = memoryManagement(MEMORY_ASIGN_CODE,sizeof(schedulerNode));
	//schedulerQueue auxN = malloc(sizeof(schedulerNode));
	auxN->content = process;

	if (isEmpty()) {
		auxN->next = auxN;
		tail = auxN;
		queue = auxN;
	} else {
		tail->next = auxN;
		tail = auxN;
		tail->next = queue;
	}

	queueSize++;

	return 1;
}

// Scheduler moves to the next process to serve.
int advance() {
	if (isEmpty()) {
		return 0;
	}

	if (!processTime()) {
		freeMemory();
		queueSize--;

		if (isEmpty()) {
			return 0;
		}
		tail->next = queue->next; // removed process is forfeited.
	} else {
		tail = queue; // Current process moves to tail of queue.
	}

	queue = queue->next; // Second process in queue becomes current process.

	setQuantum();

	return 1;
}

int processTime() {
	processPointer currentProcess = queue->content;

	currentProcess->remainingTime -= quantum;

	return currentProcess->remainingTime != 0;
}

int freeMemory() {
	//processPointer currentProcess = queue->content;

	//free(queue); // Free queue node
	//free(queue->content); // Free process node
	// Free up stack space and memory?

	return 0;
}

// Assuming there are available processes to be served, the next quantum is the default quantum
// or the remaining time for the next process before it terminates.
int setQuantum() {
	processPointer newCurrentProcess = queue->content;

	if ((newCurrentProcess->remainingTime) > QUANTUM) {
		quantum = QUANTUM;
	} else {
		quantum = (newCurrentProcess->remainingTime);
	}

	return 1;
}

/* Returns 1 if the queue is empty */
int isEmpty() {
	if(queueSize == 0) {
		return 1;
	}
	return 0;
}

/* Returns the queue size */
int size() {
	return queueSize;
}
