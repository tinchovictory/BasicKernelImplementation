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

int pcreate(void * exec, void * entryPoint, char * name) {
	return systemCall(SYS_CALL_CREATE_PROCESS, (uint64_t) exec, (uint64_t) entryPoint, (uint64_t) name);
}

int pcreateBackground(void * exec, void * entryPoint, char * name) {
	return systemCall(SYS_CALL_CREATE_PROCESS_BACKGROUND, (uint64_t) exec, (uint64_t) entryPoint, (uint64_t) name);
}

void exit() {
	pkill(getCurrentPid());
}

void pkill(int pid) {
	systemCall(SYS_CALL_END_PROCESS, pid, 0, 0);
	yield();
}

int tcreate(int pid, void * exec, void * entryPoint) {
	return (uint64_t) systemCall(SYS_CALL_CREATE_THREAD, pid, (uint64_t) exec, (uint64_t) entryPoint);
}

void tkill(int pid, int pthread) {
	systemCall(SYS_CALL_END_THREAD, pid, pthread, 0);
	yield();
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
	yield();
}

int getCurrentPid() {
	return systemCall(SYS_CALL_CURRENT_PID, 0, 0, 0);
}

int createSemaphore(int start) {
	return systemCall(SYS_CALL_CREATE_SEMAPHORE, start, 0, 0);
}

void endSemaphore(int id) {
	systemCall(SYS_CALL_END_SEMAPHORE, id, 0, 0);
}

void semaphoreUp(int id) {
	systemCall(SYS_CALL_UP_SEMAPHORE, id, 0, 0);
}

void semaphoreDown(int id) {
	systemCall(SYS_CALL_DOWN_SEMAPHORE, id, 0, 0);
	yield();
}

char * createPipe(int fromPid, int toPid){
	return (char *)systemCall(SYS_CALL_CREATE_PIPE, fromPid, toPid, 0);
}

void send(char * name, char * buff){
	systemCall(SYS_CALL_SEND,(uint64_t)name,(uint64_t)buff,0);
}

void receive(char * name, char * buff){
	systemCall(SYS_CALL_RECEIVE,(uint64_t)name,(uint64_t)buff,0);
	yield();
}

