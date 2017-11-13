#ifndef __MUTEX__
	#define __MUTEX__

	#define MUTEX_LOCKED 1
	#define MUTEX_UNLOCKED 2

	#define NULL ((void *)0)

	typedef struct blockedProcessNode {
		int pid;
		struct blockedProcessNode * next;
	} blockedProcessNode;

	typedef struct mutexNode{
		int id;
		int status;
		struct mutexNode * next;
		blockedProcessNode * blockedProcessList;
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

	void mutexRequestAccess(int id);

	void addProcessToMutexList(mutexNode * mutex, int pid);

	void unlockProcesses(int id);

#endif