#include "screenLoader.h"

static screenNode * list = NULL; // List for backup screens.
static int onFocusPid = NO_FOCUS;


// We consider an on focus process as one that can read from keyboard and has a screen
// assigned that is observable from video screen. What's more we assume there is only
// one on focus process at a time.
// 
// What loadScreen function does is to put a process on focus. 1st, it is checked if the
// process already has a screen assigned to it. If it does not, a new empty screen is
// created and assigned.
// Then, if there was a previous on focus process its screen is backed up.
// Finally, the new on focus process is loaded on video screen.
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
	setScreen(node->screen,node->currentPosition);

	onFocusPid = pid;

	return 1;
}

int isCurrentProcessOnFocus() {
	return onFocusPid == getCurrentPid();
}

int getFocusProcessPid() {
	return onFocusPid;
}

screenNode * getCurrentScreenNode() {
	screenNode * node = findScreenNodeByPid(getCurrentPid());

	if (node == NULL) {
		//ncPrint("About to add screen node for current process.");ncPrintDec(getCurrentPid());ncNewline();
		node = addScreen(getCurrentPid());

		if (node == NULL) {
			return NULL;
		}	
	}

	return node;
}

// I need the getters to the current process screen and screen pointer
// so I can call them if I want to write on its screen.
// This will happen if the video screen is unavailable (process not on focus).


char * getCurrentScreen() {
	screenNode * node = getCurrentScreenNode();

	if (node == NULL) {
		return NULL;
	}

	//screenNode * aux = list;
	//while(aux != NULL){
	//	ncPrint("nextNode,");ncPrintDec(aux->pid);ncPrint("; pos:");ncPrintHex(aux->currentPosition);ncPrint("; screen:");ncPrintHex(aux->screen);
	//	aux = aux->next;
	//}
	//ncNewline();

	return node->screen;
}

char * getCurrentScreenPosition() {
	screenNode * node = getCurrentScreenNode();

	if (node == NULL) {
		return NULL;
	}

	return node->currentPosition;
}

void setCurrentScreenPosition(char * newPosition) {
	screenNode * node = getCurrentScreenNode();

	node->currentPosition = newPosition;
}

// When I want to switch on focus screens, I need to save the video
// content of the process that was on focus until now.
// The backup goes into the my screen together with its cursor.
void saveScreen() {
	screenNode * onFocusNode = findScreenNodeByPid(onFocusPid);

	if (onFocusNode != NULL) {



		//ncPrint("--");ncPrintHex(onFocusNode);ncPrint("--");
		//ncPrint("--");ncPrintHex(onFocusNode->screen);ncPrint("--");
		//int z = 0;
		//while(z++ != 600000000);

		onFocusNode->currentPosition = backupContent(onFocusNode->screen);
	}
}

// After creating a new screen, I insert it to my list.
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

// Create a new screen node with its own screen and screen cursor.
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

	//ncPrint("New Node. screen: ");ncPrintHex(node->screen);ncPrint("; cursor: ");ncPrintHex(node->currentPosition);ncNewline();

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