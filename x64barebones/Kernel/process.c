#include <process.h>
#include <stdint.h>
#include <memoryManager.h>
#include <stackFrame.h>
#include <threads.h>
#include <videoDriver.h>

/* Number of process created */
static int currentPid = 0;

processNode * createProcess(char * name) {
	/* Create process in memory, asign a base pointer, initialize stack frame */
	processNode * process = (processNode *) allocate(PAGE_SIZE);
	process->pid = currentPid++;
	process->currentPThread = 0;
	process->threadSize = 0;
	process->state = READY;
	createProcessName(process,name);

	return process;
}

void createProcessName(processNode * process, char * name) {
	int i;

	process->name = (char *) allocate(NAME_LENGTH*sizeof(char));

	for (i = 0 ; name[i] != 0 ; i++) {
		(process->name)[i] = name[i];
	}
}

void freeProcess(processNode * process) {
	/* Free threads library */
	freeThreadLibrary(process->threadLibrary, process->threadSize);
	/* Free process pointer */
	deallocate(process, PAGE_SIZE);
}

void printProcessInfo(processNode * process) {

	print("Pid: ");printDec(process->pid);print(", status: ");print(getStatus(process->state));newLineVid();
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