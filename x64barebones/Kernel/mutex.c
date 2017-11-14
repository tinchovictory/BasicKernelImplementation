#include "mutex.h"
#include <memoryManager.h>
#include <scheduler.h>

static int counter = 0;
static mutexNode * list = NULL;


int createMutex(){
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

void mutexUp(int id) {
	updateStatus(id, MUTEX_UNLOCKED);
	unlockProcesses(id);
}

void mutexDown(int id) {
	if(mutexRequestAccess(id)) {
		updateStatus(id, MUTEX_LOCKED);
	}
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


void mutexDestroy(int id) {
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

int mutexRequestAccess(int id) {
	int locked = 1;
	mutexNode * mutex = findID(id);
	if(isLocked(id)) {
		int currentPid = getCurrentPid();
		int currentPthread = getCurrentPthread();

		addThreadToMutexList(mutex, currentPid, currentPthread);
		blockThread(currentPid, currentPthread, T_BLOCKED_MUTEX);
		locked = 0;
	}
	return locked;
}

void addThreadToMutexList(mutexNode * mutex, int pid, int pthread) {
	blockedThreadNode * node = (blockedThreadNode *)allocate(sizeof(blockedThreadNode));
	node->pid = pid;
	node->pthread = pthread;
	node->next = mutex->blockedThreadList;
	mutex->blockedThreadList = node;
}

void unlockProcesses(int id) {
	blockedThreadNode * aux;
	mutexNode * mutex = findID(id);
	while(mutex->blockedThreadList != NULL) {
		aux = mutex->blockedThreadList->next;

		unblockThread(mutex->blockedThreadList->pid, mutex->blockedThreadList->pthread, T_BLOCKED_MUTEX);
		deallocate(mutex->blockedThreadList, sizeof(blockedThreadNode));

		mutex->blockedThreadList = aux;
	}
}