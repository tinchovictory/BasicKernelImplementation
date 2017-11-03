#include <threads.h>
#include <stdint.h>
#include <stackFrame.h>
#include <memoryManager.h>
#include <process.h>
#include <scheduler.h>


int addThreadToProcess(int pid, void * entryPoint) {
	processNode * process = getProcessWithPid(pid);

	if(process == NULL) {
		return -1;
	}

	threadNode * thread = createThread(entryPoint, process->currentPThread);

	threadLibrary * node = (threadLibrary *) allocate(PAGE_SIZE);

	node->thread = thread;

	if(process->currentThread == NULL) {
		process->threadLibrary = node;
		process->currentThread = node;
		process->threadLibrary->next = process->threadLibrary;
	} else {
		node->next = process->threadLibrary->next;
		process->threadLibrary->next = node;
	}

	process->currentPThread++;
	process->threadSize++;

	return thread->pthread;
}

//static int currentPthread = 0; // TIENE QUE ESTAR EN PROCESS


threadNode * createThread(void * entryPoint, int currentPThread) {
	/* Create process in memory, asign a base pointer, initialize stack frame */
	threadNode * thread = (threadNode *) allocate(PAGE_SIZE);
	thread->entryPoint = entryPoint;
	thread->baseStack = (void *) allocate(PAGE_SIZE * NUMBER_OF_PAGES);
	thread->userStack = fillStackFrame(entryPoint, thread->baseStack);
	thread->pthread = currentPThread;
	thread->state = T_READY;

	//currentPthread++;

	return thread;
}

/* fillStackFrame taken form RowDaBoat */
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
	
}

void freeThreadLibrary(threadLibrary * library, int threadSize) {
	/* Free each thread and user stack */
	int i;
	threadLibrary * current = library, *prev;
	for(i = 0; i < threadSize; i++) {

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