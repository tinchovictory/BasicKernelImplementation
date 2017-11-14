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

	//ncPrint("Hola");ncPrintDec(getCurrentPid());ncNewline();

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

void blockProcess(int pid) {
	processNode * process = getProcessWithPid(pid);
	process->state = BLOCKED;

	numberOfTicks = QUANTUM;
}

void unblockProcess(int pid) {
	processNode * process = getProcessWithPid(pid);
	//Check for dead process
	if(process->state == BLOCKED) {
		process->state = READY;
	}
}

void blockThread(int pid, int pthread) {
	processNode * process = getProcessWithPid(pid);
	threadNode * thread = getThreadWithPthread(pthread, process);

	thread->state = T_BLOCKED;
	process->threadTick = THREAD_QUANTUM;

	/* Check if all thread of the process are blocked */
	if(allThreadsAreBlocked(process)) {
		blockProcess(pid);
	}

}

void unblockThread(int pid, int pthread) {
	processNode * process = getProcessWithPid(pid);
	threadNode * thread = getThreadWithPthread(pthread, process);

	if(thread->state == T_BLOCKED) {
		thread->state = T_READY;
	}

	unblockProcess(pid);
}


void yieldSwitch() {
	if(currentProcess == NULL) {
		return;
	}
	if(currentProcess->process->state == BLOCKED) {
		numberOfTicks = QUANTUM;
		runScheduler();
	}
}

int getCurrentPid() {
	return currentProcess->process->pid;
}

int getCurrentPthread() {
	return currentProcess->process->currentThread->thread->pthread;
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

/* - Debuging - */
void printProcessState() {
	int i;
	schedulerNode * node = processQueue;
	ncNewline();
	for(i = 0; i < queueSize; i++) {
		printProcessState(node->process);
		node = node->next;
	}
}

void printThreadStatus(threadState state){
	switch(state){
		case T_READY:
			ncPrint("READY");
		case T_RUNNING:
			ncPrint("RUNNING");
		case T_BLOCKED:
			ncPrint("BLOCKED");
		default:
			ncPrint("DEAD");
	}
}

void printThread(threadNode  * thread){
	ncPrint("Thread id: ");ncPrintDec(thread->pthread);ncNewline();
	ncPrint("Thread status: ");printThreadStatus(thread->state);ncNewline();
}

void printProcessStatus(processState state){
	switch(state){
		case READY:
			ncPrint("READY");
		case RUNNING:
			ncPrint("RUNNING");
		case BLOCKED:
			ncPrint("BLOCKED");
		default:
			ncPrint("DEAD");
	}
}

void printProcess(processNode * process){
	
	ncPrint("Process pid: "); ncPrintDec(process->pid);ncNewline();
	ncPrint("Process status: ");printProcessStatus(process->state);ncNewline();

	threadLibrary  * lib = process->threadLibrary;
	while(lib != NULL){
		ncPrint("   ");printThread(lib->thread);ncNewline();
		lib = lib->next;
	}
}

void printProcessPID(int pid){
	schedulerQueue process = processQueue;
	while(processQueue != NULL){
		if(process->process->pid == pid){
			printProcess(process->process);
			return;
		}
		process = process->next;
	}
}

void printAllProcess(int mode){
	if(processQueue == NULL){
		return;
	}
	schedulerQueue queue = processQueue;
	while(queue->process != NULL){
		printProcess(queue->process);
		queue = queue->next;
	}
}
