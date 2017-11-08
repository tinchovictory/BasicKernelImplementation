#include <process.h>
#include <stdint.h>
#include <memoryManager.h>
#include <stackFrame.h>
#include <threads.h>

#include <naiveConsole.h>

/* Number of process created */
static int currentPid = 0;

processPointer createProcess(const char* name) {
	/* Create process in memory, asign a base pointer, initialize stack frame */
	processPointer process = (processPointer) allocate(PAGE_SIZE);
	//process->name = name;
	process->pid = currentPid;
	process->currentPThread = 0;
	process->threadSize = 0;
	process->state = READY;

	currentPid++;

	return process;
}