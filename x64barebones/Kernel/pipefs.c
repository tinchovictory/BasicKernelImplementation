#include "pipefs.h"
#include <memoryManager.h>
#include <scheduler.h>

#include <naiveConsole.h>

static pipeNode * pipeList = NULL;

char * do_pipe(int fromPid, int toPid) {
	pipeNode * pipe = createPipe(fromPid, toPid);
	pipeList = addPipeToList(pipeList, pipe);
	return pipe->name;
}

pipeNode * createPipe(int fromPid, int toPid) {
	int i;

	pipeNode * pipe = (pipeNode *) allocate(sizeof(pipeNode));
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

		//ncPrint("aca");

		if(pipe->writePointer == pipe->buffer + PIPE_BUF_SIZE) {
			pipe->writePointer = pipe->buffer;
		}
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

		if(pipe->readPointer == pipe->buffer + PIPE_BUF_SIZE) {
			pipe->readPointer = pipe->buffer;
		}

	} while( pipe->readPointer != pipe->writePointer );

	*(pipe->readPointer) = 0;
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
	//TODO
	return 1;
} 

void createName(int fromPid, int toPid, char name[NAME_SIZE]) {
	char aux[NAME_SIZE] = {0};
	int i = 0, j = 0;

	if(fromPid == 0) {
		aux[i++] = '0';
	} else {
		while(fromPid != 0) {
			aux[i++] = fromPid % 10 + '0';
			fromPid /= 10;
		}
	}

	j = i;

	while(i != 0) {
		name[j-i] = aux[i-1];
		i--;
	}

	name[j++] = '-';

	if(toPid == 0) {
		aux[i++] = '0';
	} else {
		while(toPid != 0) {
			aux[i++] = toPid % 10 + '0';
			toPid /= 10;
		}	
	}
	
	j = j+i;
	while(i != 0) {
		name[j-i] = aux[i-1];
		i--;
	}
	name[j] = 0;

}

int getReceiverPid(char name[NAME_SIZE]) {
	int i=0, pid = 0;
	while(name[i] != '-') {
		i++;
	}
	i++;
	// Estoy en la primer parte del numero
	while(name[i]) {
		pid *= 10;
		pid += (name[i] - '0');
		i++;
	}

	return pid;
}