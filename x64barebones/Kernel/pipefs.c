#include "pipefs.h"
#include <memoryManager.h>
#include <scheduler.h>

#include <myStdLib.h>

static pipeNode * pipeList = NULL;

char * do_pipe(int fromPid, int toPid) {
	pipeNode * pipe = createPipe(fromPid, toPid);
	pipeList = addPipeToList(pipeList, pipe);
	return pipe->name;
}

pipeNode * createPipe(int fromPid, int toPid) {
	int i;

	pipeNode * pipe = (pipeNode *) allocate(sizeof(pipeNode));
	pipe->name = (char *) allocate(NAME_SIZE*sizeof(char));
	createName(fromPid, toPid, pipe->name);
	pipe->buffer = (char *) allocate(PIPE_BUF_SIZE);

	for(i = 0; i < PIPE_BUF_SIZE; i++) {
		(pipe->buffer)[i] = 0;
	}

	pipe->readPointer = pipe->buffer;
	pipe->writePointer = pipe->buffer;

	return pipe;
}

pipeNode * addPipeToList(pipeNode * list, pipeNode * pipe) {
	if (list == NULL) {
		return pipe;
	}
	list->next = addPipeToList(list->next, pipe);
	return list;
}

void writeToPipe(char name[NAME_SIZE], char * buff) {
	pipeNode * pipe = getPipe(name);
	
	if(pipe == NULL || buff == NULL) {
		return;
	}

	int pid = getReceiverPid(name);

	while(*buff) {

		if(*(pipe->writePointer) != 0) {
			return;
		}

		*(pipe->writePointer++) = *(buff++);

		//Unblock
		unblockProcess(pid);

		pipe->writePointer = pointerPosition(pipe->writePointer,pipe->buffer);
	}
}

void readFromPipe(char name[NAME_SIZE], char * buff) {
	pipeNode * pipe = getPipe(name);
	if(pipe == NULL || buff == NULL ) {
		return;
	}

	if(*(pipe->readPointer) == 0) {
		int pid = getReceiverPid(name);
		//Block
		blockProcess(pid);
	}

	do {

		*(buff++) = *(pipe->readPointer);
		*(pipe->readPointer++) = 0;

		pipe->readPointer = pointerPosition(pipe->readPointer,pipe->buffer);

	} while( pipe->readPointer != pipe->writePointer );

	*(pipe->readPointer) = 0;
}

char * pointerPosition (char * currentPointer, char * buffer) {
	if(currentPointer == buffer + PIPE_BUF_SIZE) {
		return buffer;
	}
	return currentPointer;
}

pipeNode * getPipe(char name[NAME_SIZE]) {
	pipeNode * current = pipeList;
	
	while(current != NULL) {
		if(equalName(name, current->name)) {
			return current;
		}
		current = current->next;
	}

	return NULL;
}

int equalName(char * str1, char * str2) {
	while (*str1 && (*str1 == *str2)) {
		str1++;
		str2++;
	}
	return *str1 == *str2;
} 

void createName(int fromPid, int toPid, char name[NAME_SIZE]) {
	int i = 0;

	i = itoa(fromPid,name);

	name[i++] = '-';

	itoa(toPid,&name[i]);
}

int getReceiverPid(char name[NAME_SIZE]) {
	int i = 0, pid = 0;
	while(name[i++] != '-'); // I move to receiver pid

	while(name[i]) {
		pid *= 10;
		pid += (name[i] - '0');
		i++;
	}

	return pid;
}