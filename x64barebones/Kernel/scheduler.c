#include <naiveConsole.h>
#include "systemCalls.h"
#include <scheduler.h>
#include <process.h>
#include <threads.h>
#include <videoDriver.h>

#include <memoryManager.h> //TESING THEN REPLACE WITH MEMORY ALLOCATOR
#include <naiveConsole.h>

#define BUFFER_SIZE 100
#define PRINT_THREADS 0

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
	processNode * process = currentProcess->process;
	return process->currentThread->thread->userStack;
}

int addProcess(void * exec, void * entryPoint, char * name) {
	processNode * process = createProcess(name, getCurrentPid());
	addProcessToQueue(process);
	addThreadToProcess(process->pid, exec, entryPoint);
	return process->pid;
}

void addProcessToQueue(processNode * p) {
	schedulerNode * node = (schedulerNode *) allocate(PAGE_SIZE);

	node->process = p;
	node->process->threadTick = 0;

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

			freeProcess(current->process);
			/* Free process slot */
			deallocate(current, PAGE_SIZE);
		}
		prev = current;
		current = current->next;
	}
}

void blockProcess(int pid, processState blockType) {
	processNode * process = getProcessWithPid(pid);
	process->state = blockType;

	numberOfTicks = QUANTUM;
}

void unblockProcess(int pid, processState blockType) {
	processNode * process = getProcessWithPid(pid);
	//Check for dead process
	if(process->state == blockType) {
		process->state = READY;
	}
}

void blockThread(int pid, int pthread, threadState blockType) {
	processNode * process = getProcessWithPid(pid);
	threadNode * thread = getThreadWithPthread(pthread, process);

	thread->state = blockType;
	process->threadTick = THREAD_QUANTUM;

	/* Check if all thread of the process are blocked */
	if(allThreadsAreBlocked(process)) {
		blockProcess(pid,getBlockProcessType(blockType));
	}
}

void unblockThread(int pid, int pthread, threadState blockType) {
	processNode * process = getProcessWithPid(pid);
	threadNode * thread = getThreadWithPthread(pthread, process);

	if(thread->state == blockType) {
		thread->state = T_READY;
	}

	unblockProcess(pid, getBlockProcessType(blockType));
}

void unblockAllThreads(int pid, threadState blockType) {
	processNode * process = getProcessWithPid(pid);
	threadLibrary * threadLib = process->threadLibrary;
	int threadsCount = 0;
	while(threadsCount < process->threadSize) {
		if(threadLib->thread->state == blockType) {
			threadLib->thread->state = T_READY;
			unblockProcess(pid, getBlockProcessType(blockType));
		}
		threadLib = threadLib->next;
		threadsCount++;
	}
}


void yieldSwitch() {
	if(currentProcess == NULL) {
		return;
	}
	if(isProcessBlocked(currentProcess->process->state) || currentProcess->process->state == DEAD ) {
		numberOfTicks = QUANTUM;
		runScheduler();
	} else if(isThreadBlocked(currentProcess->process->currentThread->thread->state) || currentProcess->process->currentThread->thread->state == T_DEAD) {
		currentProcess->process->threadTick = THREAD_QUANTUM;
		runScheduler();
	}
}

int getCurrentPid() {
	if(currentProcess == NULL) {
		return -1;
	}
	return currentProcess->process->pid;
}

int getCurrentPthread() {
	return currentProcess->process->currentThread->thread->pthread;
}

int getParentPid(int pid) {
	processNode * process = getProcessWithPid(pid);
	return process->ppid;
}

processState getBlockProcessType(threadState state) {
	switch(state) {
		case T_BLOCKED_IO:
			return BLOCKED_IO;
		case T_BLOCKED_MUTEX:
			return BLOCKED_MUTEX;
		case T_BLOCKED_PIPE:
			return BLOCKED_PIPE;
		case T_READY:
			return READY;
		case T_RUNNING:
			return RUNNING;
		case T_DEAD:
			return DEAD;
	}
	return -1;
}

int isProcessBlocked(processState state) {
	if(state == BLOCKED_IO || state == BLOCKED_PIPE || state == BLOCKED_MUTEX) {
		return 1;
	}
	return 0;
}

int isThreadBlocked(threadState state) {
	if(state == T_BLOCKED_IO || state == T_BLOCKED_PIPE || state == T_BLOCKED_MUTEX) {
		return 1;
	}
	return 0;
}


/* Threads */

void threadCheck() {
	if( (currentProcess->process->threadTick) < THREAD_QUANTUM) {
		currentProcess->process->threadTick++;
		return;
	}
	currentProcess->process->threadTick = 0;
	nextThread();
}

void nextThread() {
	/* Remove all dead thread */
	removeDeadThreads(currentProcess->process);

	/* Switch to the next thread */
	if(currentProcess->process->currentThread->thread->state == T_RUNNING) {
		currentProcess->process->currentThread->thread->state = T_READY;
	}
	
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

int getCurrentThreadOfProcess(int pid) {
	processNode * process = getProcessWithPid(pid);
	return process->currentThread->thread->pthread;
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

int allThreadsAreBlocked(processNode * process) {
	int i;
	threadLibrary * current = process->threadLibrary;
	for(i = 0; i < process->threadSize; i++) {
		if(current->thread->state == T_READY) {
			return 0;
		}
		current = current->next;
	}

	return 1;
}

/* Print */
void printAllProcess() {
	if(processQueue == NULL){
		return;
	}
	schedulerQueue queue = processQueue;
	int i;

	printPsHeader();

	while(i < queueSize){
		printProcessInfo(queue->process);
		queue = queue->next;
		i++;
	}

	newLine();newLine();newLine();
}


void printPsHeader() {

	int process = 0, blocked = 0, running = 0, ready = 0, threads = 0;

	schedulerQueue current = processQueue;
	int i;

	while(i < queueSize){
		process++;
		if(isProcessBlocked(current->process->state)) {
			blocked++;
		} else if(current->process->state == RUNNING) {
			running++;
		} else if(current->process->state == READY) {
			ready++;
		}
		threads += current->process->threadSize;

		current = current->next;
		i++;
	}

	newLine();
	print("Processses: ");
	printDec(process);print(" total, ");
	printDec(running);print(" running, ");
	printDec(ready);print(" ready, ");
	printDec(blocked);print(" blocked");
	newLine();
	print("Threads: ");
	printDec(threads);print(" total");
	newLine();newLine();
	print("PID");printTab();print("Command");printTab();print("#Threads");printTab();print("PPID");printTab();print("State");
	newLine();
}
