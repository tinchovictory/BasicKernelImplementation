#include "systemCalls.h"
#define SYS_CALL_READ 1
#define SYS_CALL_WRITE 2
#define SYS_CALL_CLEAR_SCREEN 3
#define SYS_CALL_MEMORY 4
#define SYS_CALL_PS 5

#include <stdio.h>

int read(int fildes, void *buf, unsigned int nbytes){
	int value;
	do {
		value = systemCall(SYS_CALL_READ, fildes, buf, nbytes);
		//if(value == 0) {
			//switch process
			//printf("yield\n");
			yield();
		//}
	} while(value == 0);
	//clearScreen();
	//printf("Sali\n");
	/*value = systemCall(SYS_CALL_READ, fildes, buf, nbytes);
	if(value != 0) {
		printf("En user es: %d\n", value);
	}*/
	return value; 
}

int write(int fildes, void *buf, unsigned int nbytes){
	return systemCall(SYS_CALL_WRITE, fildes, buf, nbytes);
}

void clearScreen(){
	systemCall(SYS_CALL_CLEAR_SCREEN, 1, 0, 0);
}

void * memoryManagement(int memoryCode, unsigned int nbytes){
	return (void *) systemCall(SYS_CALL_MEMORY,memoryCode,0,nbytes);
}

void ps(){
	systemCall(SYS_CALL_PS, 1, 0, 0);
}