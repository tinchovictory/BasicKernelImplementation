#include "mutex.h"
#include <memoryManager.h>
#include <scheduler.h>

static int counter = 0;
static mutexNode * list = NULL;


int create(){
	mutexNode * mutex = allocate(sizeof(mutexNode));

	mutex->id = counter++;
	mutex->status = MUTEX_UNLOCKED;

	list = addMutex(list, mutex);
	
	return mutex->id;
}

mutexNode * addMutex(mutexNode * currentNode, mutexNode * mutex) {
	if(currentNode == NULL){
		return mutex;
	}
	currentNode->next = addMutex(currentNode->next, mutex);
	return currentNode;
}

void up(int id) {
	updateStatus(id, MUTEX_UNLOCKED);
	unlockProcesses(id);
}

void down(int id) {
	updateStatus(id, MUTEX_LOCKED);
}

void updateStatus(int id, int mutexStatus) {
	mutexNode * mutex = findID(id);
	if(mutex != NULL){
		mutex->status = mutexStatus;
	}
}

mutexNode * findID(int id) {
	mutexNode * aux = list;
	while(aux != NULL && aux->id != id){
		aux = aux->next;
	}
	return aux;
}


void destroy(int id) {
	list = destroyMutex(list, id);
}

mutexNode * destroyMutex(mutexNode * currentNode, int mutexId) {
	if (currentNode == NULL) {
		return NULL;
	}
	if(currentNode->id == mutexId) {
		mutexNode * aux = currentNode->next;
		deallocate(currentNode, sizeof(mutexNode));
		return aux;
	}
	currentNode->next = destroyMutex(currentNode->next, mutexId);
	return currentNode;
}

int isLocked(int id) {
	mutexNode * mutex = findID(id);
	if(mutex == NULL){
		return -1;
	}
	return mutex->status == MUTEX_LOCKED;
}

void requestAccess(int id) {
	mutexNode * mutex = findID(id);
	while(isLocked(id)) {
		int currentPid = getCurrentPid();
		
		addProcessToMutexList(mutex, currentPid);
		blockProcess(currentPid);
	}
}

void addProcessToMutexList(mutexNode * mutex, int pid) {
	blockedProcessNode * node = (blockedProcessNode *)allocate(sizeof(blockedProcessNode));
	node->pid = pid;
	node->next = mutex->blockedProcessList;
	mutex->blockedProcessList = node;
}

void unlockProcesses(int id) {
	blockedProcessNode * aux;
	mutexNode * mutex = findID(id);
	while(mutex->blockedProcessList != NULL) {
		aux = mutex->blockedProcessList->next;

		unblockProcess(mutex->blockedProcessList->pid);
		deallocate(mutex->blockedProcessList, sizeof(blockedProcessNode));

		mutex->blockedProcessList = aux;
	}
}