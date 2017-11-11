#include "screenLoader.h"

static screenNode * list = NULL;
static char * onFocusScreen = NULL;

int loadScreen(int pid) {
	char * screen = findScreenByPid(pid);

	// If the pid has no screen data assigned, I create one.
	if (screen == NULL) {
		screen = addScreen(pid);

		if (screen == NULL) {
			return 0;
		}	
	}
	saveScreen(); // Save previous on focus video data for future use.

	setScreen(screen); // Move the new on focus video data to the screen.

	onFocusScreen = screen;

	return 1;
}

void saveScreen() {
	if (onFocusScreen != NULL) {
		transferContent(onFocusScreen); // Save content currently stored in the video.
	}
}

char * addScreen(int pid) {
	screenNode * node = (screenNode *)allocate(sizeof(screenNode));

	if (node == NULL) {
		return NULL;
	}

	node->screen = newScreen();

	if (node->screen == NULL) {
		return NULL;
	}
	node->pid = pid;
	node->next = list;
	list = node;

	return node->screen;
}

char * newScreen() {
	char * screen = (char *)allocate(WINDOW_SIZE*sizeof(char *));

	if (screen == NULL) {
		return NULL;
	}
	clear(screen);

	return screen;
}

char * findScreenByPid(int pid) {
	screenNode * node = findScreenNodeByPid(pid);

	if (node == NULL) {
		return NULL;
	}
	return node->screen;
}

screenNode * findScreenNodeByPid(int pid) {
	screenNode * node = list;

	while(node != NULL && node->pid != pid){
		node = node->next;
	}
	return node;
}

void removeScreen(int pid) {
	if (findScreenByPid(pid) == onFocusScreen) {
		onFocusScreen = NULL;
	}
	removeScreenInner(list,pid);
}

screenNode * removeScreenInner(screenNode * currentNode, int pid) {
	if (currentNode == NULL) {
		return NULL;
	}
	if(currentNode->pid == pid) {
		screenNode * aux = currentNode->next;
		deallocate(currentNode->screen, WINDOW_SIZE*sizeof(char *));
		deallocate(currentNode, sizeof(screenNode));

		return aux;
	}
	currentNode->next = removeScreenInner(currentNode->next, pid);
	return currentNode;
}