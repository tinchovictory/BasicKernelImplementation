#ifndef __SCREEN_LOADER__
	#define __SCREEN_LOADER__

	#include <videoDriver.h>
	#include <memoryManager.h>

	typedef struct screenNode {
		int pid;
		char * screen;
		struct screenNode * next;
	} screenNode;

	int loadScreen(int pid);
	void saveScreen();
	char * addScreen(int pid);
	char * newScreen();

	char * findScreenByPid(int pid);
	screenNode * findScreenNodeByPid(int pid);

	void removeScreen(int pid);
	screenNode * removeScreenInner(screenNode * currentNode, int pid);

#endif