#ifndef __MUTEX__
	#define __MUTEX__

	#define MUTEX_LOCKED 1
	#define MUTEX_UNLOCKED 2

	#define NULL ((void *)0)

	typedef struct blockedThreadNode {
		int pid;
		int pthread;
		struct blockedThreadNode * next;
	} blockedThreadNode;

	typedef struct mutexNode{
		int id;
		int status;
		struct mutexNode * next;
		blockedThreadNode * blockedThreadList;
	} mutexNode;

	int createMutex();

	mutexNode * addMutex(mutexNode * currentNode, mutexNode * mutex);

	mutexNode * findID(int id);

	void mutexUp(int id);

	void mutexDown(int id);

	void updateStatus(int id, int mutexStatus);

	void mutexDestroy(int id);

	mutexNode * destroyMutex(mutexNode * current, int mutexId);

	int isLocked(int id);

	int mutexRequestAccess(int id);

	void addThreadToMutexList(mutexNode * mutex, int pid, int pthread);

	void unlockProcesses(int id);

#endif