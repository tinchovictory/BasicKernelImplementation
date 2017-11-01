#include <process.h>
#include <stdint.h>
#include <memoryManager.h>
#include <stackFrame.h>

#include <naiveConsole.h>

/* Number of process created */
static int currentPid = 0;

processPointer createProcess(void * entryPoint) {
	/* Create process in memory, asign a base pointer, initialize stack frame */
	processPointer process = (processPointer) allocate(PAGE_SIZE);
	process->entryPoint = entryPoint;
	process->baseStack = (void *) allocate(PAGE_SIZE * NUMBER_OF_PAGES);
	process->userStack = fillStackFrame(entryPoint, process->baseStack);
	process->pid = currentPid;

	currentPid++;

	return process;
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