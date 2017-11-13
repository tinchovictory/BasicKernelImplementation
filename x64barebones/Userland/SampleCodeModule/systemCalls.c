#include "systemCalls.h"

#include <stdio.h>

int read(int fildes, void *buf, unsigned int nbytes){
	int value;
	do {
		value = systemCall(SYS_CALL_READ, fildes, (uint64_t) buf, nbytes);
		yield();
	} while(value == 0);
	
	return value; 
}

int write(int fildes, void *buf, unsigned int nbytes){
	return systemCall(SYS_CALL_WRITE, fildes, (uint64_t) buf, nbytes);
}

void clearScreen(){
	systemCall(SYS_CALL_CLEAR_SCREEN, 0, 0, 0);
}

void * malloc(int bytes) {
	return (void *) systemCall(SYS_CALL_MEMORY_ASSIGN, bytes, 0, 0);
}

void free(void * memPosition) {
	systemCall(SYS_CALL_MEMORY_FREE, (uint64_t) memPosition, 0, 0);
}

void ps(){
	systemCall(SYS_CALL_PS, 1, 0, 0);
}

int pcreate(void * entryPoint) {
	return systemCall(SYS_CALL_CREATE_PROCESS, (uint64_t) entryPoint, 0, 0);
}

void pkill(int pid) {
	systemCall(SYS_CALL_END_PROCESS, pid, 0, 0);
}

int tcreate(int pid, void * entryPoint) {
	return (uint64_t) systemCall(SYS_CALL_CREATE_THREAD, pid, (uint64_t) entryPoint, 0);
}

void tkill(int pid, int pthread) {
	systemCall(SYS_CALL_END_THREAD, pid, pthread, 0);
}

int createMutex() {
	return systemCall(SYS_CALL_CREATE_MUTEX, 0, 0, 0);
}

void endMutex(int id) {
	systemCall(SYS_CALL_END_MUTEX, id, 0, 0);
}

void mutexUp(int id) {
	systemCall(SYS_CALL_UP_MUTEX, id, 0, 0);	
}

void mutexDown(int id) {
	systemCall(SYS_CALL_DOWN_MUTEX, id, 0, 0);
}

int getCurrentPid() {
	return systemCall(SYS_CALL_CURRENT_PID, 0, 0, 0);
}







/*
void * memoryManagement(int memoryCode, unsigned int nbytes){
	return (void *) systemCall(SYS_CALL_MEMORY,memoryCode,0,nbytes);
}
*/
