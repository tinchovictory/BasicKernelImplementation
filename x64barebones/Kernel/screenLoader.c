#include "screenLoader.h"

static screenNode * list = NULL;
static int onFocusPid = NO_FOCUS;

int loadScreen(int pid) {
	screenNode * node = findScreenNodeByPid(pid);

	// If the pid has no screen data assigned, I create one.
	if (node == NULL) {
		node = addScreen(pid);

		if (node == NULL) {
			return 0;
		}	
	}
	saveScreen(); // Save previous on focus video data for future use.
	// Move the new on focus video data to the screen.
	node->currentPosition = setScreen(node->screen);

	onFocusPid = pid;

	return 1;
}

int isCurrentProcessOnFocus() {
	return onFocusPid == getCurrentPid();
}

char * getCurrentScreen() {
	screenNode * node = findScreenNodeByPid(getCurrentPid());

	if (node == NULL) {
		node = addScreen(getCurrentPid());

		if (node == NULL) {
			return NULL;
		}	
	}
	return node;
}

char * getCurrentScreenPosition() {
	screenNode * node = getCurrentScreen();

	if (node == NULL) {
		return NULL;
	}
	return node->currentPosition;
}

void saveScreen() {
	if (onFocusPid != NO_FOCUS) {
		// Save content currently stored in the video.
		transferContent(findScreenByPid(onFocusPid));
	}
}

screenNode * addScreen(int pid) {
	screenNode * node = newScreenNode();

	if (node == NULL) {
		return NULL;
	}
	node->pid = pid;
	node->next = list;
	list = node;

	return node;
}

screenNode * newScreenNode() {
	screenNode * node = (screenNode *)allocate(sizeof(screenNode));

	if (node == NULL) {
		return NULL;
	}

	node->screen = (char *)allocate(WINDOW_SIZE*sizeof(char *));

	if (node->screen == NULL) {
		deallocate(node, sizeof(screenNode));
		return NULL;
	}
	clear(node->screen);

	node->currentPosition = node->screen;

	return node;
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
	if (pid == onFocusPid) {
		onFocusPid = NO_FOCUS;
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