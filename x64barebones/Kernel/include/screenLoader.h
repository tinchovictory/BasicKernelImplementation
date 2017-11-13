#ifndef __SCREEN_LOADER__
	#define __SCREEN_LOADER__

	#include <videoDriver.h>
	#include <memoryManager.h>
	#include <scheduler.h>

	#define NO_FOCUS -1

	typedef struct screenNode {
		int pid;
		char * screen;
		char * currentPosition;
		struct screenNode * next;
	} screenNode;

	int loadScreen(int pid);

	int isCurrentProcessOnFocus();

	int getFocusProcessPid();

	screenNode * getCurrentScreenNode();
	char * getCurrentScreen();
	char * getCurrentScreenPosition();

	void setCurrentScreenPosition(char * newPosition);

	void saveScreen();
	
	screenNode * addScreen(int pid);
	screenNode * newScreenNode();

	char * findScreenByPid(int pid);
	screenNode * findScreenNodeByPid(int pid);

	void removeScreen(int pid);
	screenNode * removeScreenInner(screenNode * currentNode, int pid);

#endif