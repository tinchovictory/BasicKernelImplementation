#include "semaphores.h"
#include <memoryManager.h>
#include <scheduler.h>

#include <naiveConsole.h>

static int counter = 0;
static semaphoreNode * list = NULL;


int createSemaphore(int start) {
	semaphoreNode * semaphore = allocate(sizeof(semaphoreNode));

	semaphore->id = counter++;
	semaphore->status = start;

	list = addSemaphore(list, semaphore);
	
	return semaphore->id;
}

semaphoreNode * addSemaphore(semaphoreNode * currentNode, semaphoreNode * semaphore) {
	if(currentNode == NULL){
		return semaphore;
	}
	currentNode->next = addSemaphore(currentNode->next, semaphore);
	return currentNode;
}

void semaphoreUp(int id) {
	updateSemaphoreStatus(id, SEMAPHORE_UP);
	if(!isSemaphoreLocked(id)) {
		unlockSemaphoreProcess(id);
	}
}

void semaphoreDown(int id) {
	updateSemaphoreStatus(id, SEMAPHORE_DOWN);
	semaphoreRequestAccess(id);
}

void updateSemaphoreStatus(int id, semaphoreActions action) {
	semaphoreNode * semaphore = findSemaphoreId(id);
	if(semaphore == NULL){
		return;
	}
	//ncPrint("                                     ");ncPrintDec(semaphore->status);ncPrint(", id: ");ncPrintDec(id);ncNewline();
	if(action == SEMAPHORE_DOWN) {
		semaphore->status -= 1;
	} else if(action == SEMAPHORE_UP) {
		semaphore->status += 1;
	}
}

semaphoreNode * findSemaphoreId(int id) {
	semaphoreNode * aux = list;
	while(aux != NULL && aux->id != id){
		aux = aux->next;
	}
	return aux;
}


void semaphoreDestroy(int id) {
	list = destroySemaphore(list, id);
}

semaphoreNode * destroySemaphore(semaphoreNode * currentNode, int semaphoreId) {
	if (currentNode == NULL) {
		return NULL;
	}
	if(currentNode->id == semaphoreId) {
		semaphoreNode * aux = currentNode->next;
		deallocate(currentNode, sizeof(semaphoreNode));
		return aux;
	}
	currentNode->next = destroySemaphore(currentNode->next, semaphoreId);
	return currentNode;
}

int isSemaphoreLocked(int id) {
	semaphoreNode * semaphore = findSemaphoreId(id);
	if(semaphore == NULL){
		return -1;
	}
	return semaphore->status < 0;
}

int semaphoreRequestAccess(int id) {
	int locked = 1;
	semaphoreNode * semaphore = findSemaphoreId(id);
	if(isSemaphoreLocked(id)) {
		int currentPid = getCurrentPid();
		int currentPthread = getCurrentPthread();

		addThreadToSemaphoreList(semaphore, currentPid, currentPthread);
		blockThread(currentPid, currentPthread, T_BLOCKED_MUTEX);
		locked = 0;
	}
	return locked;
}

void addThreadToSemaphoreList(semaphoreNode * semaphore, int pid, int pthread) {
	blockedThreadNodeS * node = (blockedThreadNodeS *)allocate(sizeof(blockedThreadNodeS));
	node->pid = pid;
	node->pthread = pthread;
	node->next = semaphore->blockedThreadList;
	semaphore->blockedThreadList = node;
}

void unlockSemaphoreProcess(int id) {
	blockedThreadNodeS * aux;
	semaphoreNode * semaphore = findSemaphoreId(id);
	while(semaphore->blockedThreadList != NULL) {
		aux = semaphore->blockedThreadList->next;

		unblockThread(semaphore->blockedThreadList->pid, semaphore->blockedThreadList->pthread, T_BLOCKED_MUTEX);
		deallocate(semaphore->blockedThreadList, sizeof(blockedThreadNodeS));

		semaphore->blockedThreadList = aux;
	}
}