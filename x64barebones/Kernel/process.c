#include <process.h>
#include <stdint.h>
#include <memoryManager.h>

#include <naiveConsole.h>

/* Number of process created */
static int currentPid = 0;

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


processPointer createProcess(void * entryPoint) {
	/* Create process in memory, asign a base pointer, initialize stack frame */
	processPointer process = (processPointer) allocate(0x1000);
	process->entryPoint = entryPoint;
	process->baseStack = (void *) allocate(0x100000);
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