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

int quantumCheck() {
	if(numberOfTicks < QUANTUM) {
		numberOfTicks++;
		return threadCheck();
		//return 0;
	}
	numberOfTicks = 0;
	return 1;
}

void * switchUserToKernel(void * esp) {
	currentProcess->process->userStack = esp; // sin esto no anda
	return kernelStack;
}

void runScheduler() {
	if(currentProcess == NULL) {
		return;
	}

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
	schedulerNode * node = (schedulerNode *) allocate(0x1000);

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
	return currentProcess->process->entryPoint;
}

void setKernelStack() {
	kernelStack = (void *) allocate(0x1000);
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


/* PASASR A OTRO ARCHIVO */


int threadCheck() {
	if(currentProcess->process->threadLibrary == NULL) {
		return 0;
	}

	if( (currentProcess->threadTick) < THREAD_QUANTUM) {
		currentProcess->threadTick++;
		return 0;
	}
	ncPrint("Thread Switch");
	currentProcess->threadTick = 0;
	//threadSwitch();
	return 2;
}

void * nextThread(void * rsp) {
	currentProcess->process->currentThread->thread->userStack = rsp;
	currentProcess->process->currentThread = currentProcess->process->currentThread->next;
	return currentProcess->process->currentThread->thread->userStack;
}





threadNode * createThread(void * entryPoint);
void * fillStackFrame(void * entryPoint, void * baseStack);

int addThreadToProcess(int pid, void * entryPoint) {
	threadNode * thread = createThread(entryPoint);
	//addProcessToQueue(process);


	threadLibrary * node = (threadLibrary *) allocate(0x1000);

	node->thread = thread;

//Buscar process con pid
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

/* Stack frame structure taken from RowDaBoat */
typedef struct StackFrame {
	//Registers restore context
	uint64_t gs;
	uint64_t fs;
	uint64_t r15;
	uint64_t r14;
	uint64_t r13;
	uint64_t r12;
	uint64_t r11;
	uint64_t r10;
	uint64_t r9;
	uint64_t r8;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t rdx;
	uint64_t rcx;
	uint64_t rbx;
	uint64_t rax;

	//iretq hook
	uint64_t rip;
	uint64_t cs;
	uint64_t eflags;
	uint64_t rsp;
	uint64_t ss;
	uint64_t base;
} StackFrame;


threadNode * createThread(void * entryPoint) {
	/* Create process in memory, asign a base pointer, initialize stack frame */
	threadNode * thread = (threadNode *) allocate(0x1000);
	thread->entryPoint = entryPoint;
	thread->baseStack = (void *) allocate(0x100000);
	thread->userStack = fillStackFrame(entryPoint, thread->baseStack);
	thread->pthread = currentPthread;

	currentPthread++;

	return thread;
}

/* fillStackFrame taken form RowDaBoat *//*
void * fillStackFrame(void * entryPoint, void * baseStack) {
	StackFrame * frame = (StackFrame*)baseStack - 1;
	frame->gs =		0x001;
	frame->fs =		0x002;
	frame->r15 =	0x003;
	frame->r14 =	0x004;
	frame->r13 =	0x005;
	frame->r12 =	0x006;
	frame->r11 =	0x007;
	frame->r10 =	0x008;
	frame->r9 =		0x009;
	frame->r8 =		0x00A;
	frame->rsi =	0x00B;
	frame->rdi =	0x00C;
	frame->rbp =	0x00D;
	frame->rdx =	0x00E;
	frame->rcx =	0x00F;
	frame->rbx =	0x010;
	frame->rax =	0x011;
	frame->rip =	(uint64_t)entryPoint;
	frame->cs =		0x008;
	frame->eflags = 0x202;
	frame->rsp =	(uint64_t)&(frame->base);
	frame->ss = 	0x000;
	frame->base =	0x000;

	return (void *) frame;
	
}*/