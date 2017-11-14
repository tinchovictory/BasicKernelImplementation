#include <process.h>
#include <stdint.h>
#include <memoryManager.h>
#include <stackFrame.h>
#include <threads.h>
#include <videoDriver.h>
#include <scheduler.h>

/* Number of process created */
static int currentPid = 0;

processNode * createProcess(char * name, int ppid) {
	/* Create process in memory, asign a base pointer, initialize stack frame */
	processNode * process = (processNode *) allocate(PAGE_SIZE);
	process->pid = currentPid++;
	process->ppid = ppid;
	process->currentPThread = 0;
	process->threadSize = 0;
	process->state = READY;
	createProcessName(process,name);

	process->heap = NULL;

	return process;
}

void createProcessName(processNode * process, char * name) {
	int i;

	process->name = (char *) allocate(NAME_LENGTH*sizeof(char));

	for (i = 0 ; name[i] != 0 && i < NAME_LENGTH-1 ; i++) {
		(process->name)[i] = name[i];
	}
	(process->name)[i] = 0;
}

void freeProcess(processNode * process) {
	/* Free threads library */
	freeThreadLibrary(process->threadLibrary, process->threadSize);

	freeHeap(process->heap);
	/* Free process pointer */
	deallocate(process, PAGE_SIZE);
}

void printProcessInfo(processNode * process) {
	printDec(process->pid);printTab();print(process->name);printTab();printDec(process->threadSize);printTab();printDec(process->ppid);printTab();print(getStatus(process->state));
	newLine();
}

char * getStatus(processState state) {
	switch(state) {
		case READY: return "READY";
		case RUNNING: return "RUNNING";
		case BLOCKED_IO:case BLOCKED_MUTEX:case BLOCKED_PIPE: return "BLOCKED";
		case DEAD: return "DEAD";
	}
	return NULL;
}


void * pmalloc(int nBytes) {
	void * mem = allocate(nBytes);
	processNode * current = getProcessWithPid(getCurrentPid());
	current->heap = addHeap(current->heap, mem, nBytes);
	return mem;
}

heapNode * addHeap(heapNode * node, void * mem, int nBytes) {
	if(node == NULL) {
		heapNode * newNode = (heapNode *) allocate(sizeof(heapNode));
		newNode->address = mem;
		newNode->size = nBytes;
		return newNode;
	}
	node->next = addHeap(node->next, mem, nBytes);
	return node;
}


void pfree(void * mem) {
	processNode * current = getProcessWithPid(getCurrentPid());
	current->heap = pfreeR(current->heap, mem);
}

heapNode * pfreeR(heapNode * node, void * mem) {
	if(node == NULL) {
		return NULL;
	}
	if(node->address == mem) {
		heapNode * next = node->next;
		deallocate(node->address, node->size);
		deallocate(node, sizeof(heapNode));
		return next;
	}
	node->next = pfreeR(node->next, mem);
	return node;
}

void freeHeap(heapNode * node) {
	if(node == NULL) {
		return;
	}
	heapNode * next = node->next;
	deallocate(node->address, node->size);
	deallocate(node, sizeof(heapNode));
	freeHeap(next);
}


