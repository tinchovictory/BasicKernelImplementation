#ifndef __SEMAPHORES__
#define __SEMAPHORES__

	#define NULL ((void *)0)

	typedef enum {SEMAPHORE_UP, SEMAPHORE_DOWN} semaphoreActions;

	typedef struct blockedThreadNodeS {
		int pid;
		int pthread;
		struct blockedThreadNodeS * next;
	} blockedThreadNodeS;

	typedef struct semaphoreNode{
		int id;
		int status;
		struct semaphoreNode * next;
		blockedThreadNodeS * blockedThreadList;
	} semaphoreNode;

	int createSemaphore(int start);

	semaphoreNode * addSemaphore(semaphoreNode * currentNode, semaphoreNode * semaphore);

	void semaphoreUp(int id);

	void semaphoreDown(int id);

	void updateSemaphoreStatus(int id, semaphoreActions action);

	semaphoreNode * findSemaphoreId(int id);

	void semaphoreDestroy(int id);

	semaphoreNode * destroySemaphore(semaphoreNode * currentNode, int semaphoreId);

	int isSemaphoreLocked(int id);

	int semaphoreRequestAccess(int id);

	void addThreadToSemaphoreList(semaphoreNode * semaphore, int pid, int pthread);

	void unlockSemaphoreProcess(int id);


#endif