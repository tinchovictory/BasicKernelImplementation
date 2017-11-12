#include <naiveConsole.h>
#include "systemCalls.h"
#include <scheduler.h>
#include <process.h>
#include <threads.h>
#include <videoDriver.h>

#include <memoryManager.h> //TESING THEN REPLACE WITH MEMORY ALLOCATOR
#include <naiveConsole.h>

#define BUFFER_SIZE 100

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

	ncPrint("Hola");ncPrintDec(getCurrentPid());ncNewline();

	/* Check quantum */
	if(numberOfTicks < QUANTUM) {
		numberOfTicks++;
		/* Check thread quantum */
		threadCheck();
		return;
	}

	/* Remove all dead process */
	removeDeadProcess();

	/* Run next process */
	numberOfTicks = 0;

	if(currentProcess->process->state == RUNNING) {
		currentProcess->process->state = READY;
	}

	while(currentProcess->next->process->state != READY) {
		currentProcess = currentProcess->next;
	}
	currentProcess = currentProcess->next;

	currentProcess->process->state = RUNNING;
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

void removeProcess(int pid) {
	if(pid  == 0 || pid == 1) {
		return;
	}
	processNode * process = getProcessWithPid(pid);
	process->state = DEAD;
	queueSize--;
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

void removeDeadProcess() {
	int i;
	schedulerNode * current = processQueue, *prev;
	for(i = 0; i < queueSize; i++) {
		if(current->process->state == DEAD) {

			/* Remove process */
			prev->next = current->next;
			/* Free threads library */
			freeThreadLibrary(current->process->threadLibrary, current->process->threadSize);
			/* Free process */
			deallocate(current->process, PAGE_SIZE);
			/* Free process slot */
			deallocate(current, PAGE_SIZE);


		}
		prev = current;
		current = current->next;
	}
}

void blockProcess(int pid) {
	processNode * process = getProcessWithPid(pid);
	process->state = BLOCKED;

	numberOfTicks = QUANTUM;
	while(process->state == BLOCKED);
}

void unblockProcess(int pid) {
	processNode * process = getProcessWithPid(pid);
	process->state = READY;	
}

int getCurrentPid() {
	return currentProcess->process->pid;
}

/*
void printAllProcess() {
	schedulerNode * node = processQueue;
	while(node != NULL && node->process->pid == 0)
		node = node->next;
	
	if(queueSize < 1){
		return;
	}
	do{
		if(node->process->pid){
			print("Name: "); print(node->process->name); printTab(); print("PID: "); print(strnum(node->process->pid,char str[BUFFER_SIZE],10)); printTab();
			print("State: "); print(getStatus(node->process->state)); printTab(); print("Threads: "); print(strnum(node->process->threadSize,char str1[BUFFER_SIZE],10)); newLine();
		}
		node = node->next;
	}while(node->process->pid != currentProcess->process->pid);
}
*/


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
	/* Remove all dead thread */
	removeDeadThreads(currentProcess->process);

	/* Switch to the next thread */
	currentProcess->process->currentThread->thread->state = T_READY;
	
	while(currentProcess->process->currentThread->next->thread->state != T_READY) {
		currentProcess->process->currentThread = currentProcess->process->currentThread->next;
	}
	currentProcess->process->currentThread = currentProcess->process->currentThread->next;
	currentProcess->process->currentThread->thread->state = T_RUNNING;
}

void removeThread(int pid, int pthread) {
	if(pid == 0 || pid == 1) {
		return;
	}
	if(pthread == 0) {
		return;
	}
	processNode * process = getProcessWithPid(pid);
	if(process->threadSize == 1) {
		removeProcess(pid);
	}
	/* Remove thread */
	threadNode * thread = getThreadWithPthread(pthread, process);
	thread->state = DEAD;
	process->threadSize--;
}


threadNode * getThreadWithPthread(int pthread, processNode * process) {
	threadLibrary * aux = process->threadLibrary->next;
	while(aux != process->threadLibrary && aux->thread->pthread != pthread) {
		aux = aux->next;
	}

	if(aux->thread->pthread == pthread) {
		return aux->thread;
	}

	return NULL;
}


void removeDeadThreads(processNode * process) {
	int i;
	threadLibrary * current = process->threadLibrary, *prev;

	for(i = 0; i < process->threadSize; i++) {
		if(current->thread->state == T_DEAD) {
			/* Remove thread */
			prev->next = current->next;

			/* Free user stack */
			deallocate(current->thread->baseStack, PAGE_SIZE * NUMBER_OF_PAGES);
			/* Free thread */
			deallocate(current->thread, PAGE_SIZE);
			/* Free thread slot */
			deallocate(current, PAGE_SIZE);


			prev = current;
			current = current->next;
		}
	}
}

/* - Debuging - */
void printProcessStatus() {
	int i;
	schedulerNode * node = processQueue;
	ncNewline();
	for(i = 0; i < queueSize; i++) {
		ncPrint("Pid: ");ncPrintDec(node->process->pid);ncPrint(" status: ");ncPrint(getStatus(node->process->state));ncNewline();
		node = node->next;
	}
}

char * getStatus(processState state) {
	switch(state) {
		case READY: return "READY";
		case RUNNING: return "RUNNING";
		case BLOCKED: return "BLOCKED";
		case DEAD: return "DEAD";
	}
	return NULL;
}