#include "systemCalls.h"
#include <videoDriver.h>
#include <keyBoardDriver.h>
#include <RTL8139.h>
#include <scheduler.h>
#include <screenLoader.h>
#include <malloc.h>
#include <mutex.h>

int writeToVideo(void * buf, uint64_t nBytes);
int writeToMyScreen(void * buf, uint64_t nBytes);

uint64_t clearScreenSys(){
	clearScreen();
	return 0;
}

uint64_t read(uint64_t fileDescriptor, void * buf, uint64_t nBytes){
	if(fileDescriptor == STANDARD_IO_FD){
		if (blockIfNotOnFocus()) {
			return 0;
		};

		char * myBuf = (char *) buf;
		int cont = 1, readBytes=0;
		for (int i = 0; i < nBytes && cont; i++){
			*(myBuf + i) = (char) getKey();
			if(*(myBuf + i) == 0){
				cont = 0;
			}else{
				readBytes++;
			}
		}
		
		if(readBytes == 0) {
			//blockProcess(getCurrentPid());
			blockThread(getCurrentPid(),getCurrentPthread());
		}
		return readBytes;
	}else if(fileDescriptor == ETHERNET_FD){
		return getMsg((ethMsg *) buf);
	}
	return -1;
}

int blockIfNotOnFocus(){
	if (!isCurrentProcessOnFocus()) {
		//blockProcess(getCurrentPid());
		blockThread(getCurrentPid(),getCurrentPthread());
		return 1;
	}
	return 0;
}


uint64_t write(uint64_t fileDescriptor, void * buf, uint64_t nBytes){
	if(fileDescriptor == STANDARD_IO_FD){
		if (isCurrentProcessOnFocus()) {
			return writeToVideo(buf,nBytes);
		} else {
			return writeToMyScreen(buf,nBytes);
		}
	}else if(fileDescriptor == ETHERNET_FD){
		sendMsg(*((ethMsg *) buf));
		return nBytes;
	}
	return -1;
}

int writeToVideo(void * buf, uint64_t nBytes) {
	char * myBuf = (char *) buf;
	int i;
	for(i = 0; i < nBytes && myBuf[i] != 0; i++){
		printCharacters(myBuf[i]);
	}
	return i;
}

int writeToMyScreen(void * buf, uint64_t nBytes) {

	char * myBuf = (char *) buf;
	char * myScreen = getCurrentScreen();
	char * myScreenPos = getCurrentScreenPosition();
	int i;

	for(i = 0; i < nBytes && myBuf[i] != 0; i++){
		printCharactersInner(myBuf[i],myScreen,&myScreenPos);
	}

	setCurrentScreenPosition(myScreenPos);

	return i;
}

/*
uint64_t memoryManagement(uint64_t fnCode, uint64_t nBytes){
	if(fnCode == MEMORY_ASIGN_CODE){//asigno memoria
		void * ptr = memory;
		memory += nBytes;
		return (uint64_t) ptr;
	}else if(fnCode == MEMORY_FREE_CODE){//libero memoria
		return 0;
	}
	return -1;
}

uint64_t ps() {
	PrintAllProcess();
	return 1;
}*/


uint64_t pcreate(void * entryPoint){
	int pid = addProcess(entryPoint);
	loadScreen(pid);
	return pid;
}

uint64_t pcreateBackground(void * entryPoint) {
	return addProcess(entryPoint);
}

uint64_t tcreate(uint64_t pid, void * entryPoint){
	return addThreadToProcess(pid, entryPoint);
}

uint64_t pkill(uint64_t pid){
	removeScreen(pid);
	removeProcess(pid);
	return 1;
}

uint64_t tkill(uint64_t pid, uint64_t pthread){
	removeThread(pid, pthread);
	return 1;
}
/*
uint64_t ls(uint64_t pid){
	printProcessPID(pid);
	return 1;
}
*/
uint64_t ps(){
	printAllProcess();
	return 1;
}

void * mallocSysCall(uint64_t bytes) {
	return malloc(bytes);
}

uint64_t freeSysCall(void * memoryPosition) {
	free(memoryPosition);
	return 1;
}

uint64_t createMutexSysCall() {
	return createMutex();
}

uint64_t endMutexSysCall(uint64_t id) {
	mutexDestroy(id);
	return 1;
}

uint64_t upMutexSysCall(uint64_t id) {
	mutexUp(id);
	return 1;
}

uint64_t downMutexSysCall(uint64_t id) {
	mutexDown(id);
	return 1;
}

uint64_t getCurrentPidSysCall() {
	return getCurrentPid();
}

uint64_t systemCall(uint64_t systemCallNumber, uint64_t param1, uint64_t param2, uint64_t param3){

	switch(systemCallNumber) {
		case SYS_CALL_READ:
			/* param1: filedescriptor, param2: buffer, param3: nBytes */
			return read(param1, (void *) param2, param3);

		case SYS_CALL_WRITE:
			/* param1: filedescriptor, param2: buffer, param3: nBytes */
			return write(param1, (void *) param2, param3);

		case SYS_CALL_CLEAR_SCREEN:
			/* no params */
			return clearScreenSys();

		case SYS_CALL_MEMORY_ASSIGN:
			/* param1: nBytes */
			return (uint64_t) mallocSysCall(param1);

		case SYS_CALL_MEMORY_FREE:
			/* param1: memoryPosition */
			return freeSysCall((void *) param1);

		case SYS_CALL_PS:
			/*  */
			return 1;//HAY QUE HACERLO

		case SYS_CALL_CREATE_PROCESS:
			/* param1: entryPoint */
			return pcreate((void *) param1);

		case SYS_CALL_CREATE_PROCESS_BACKGROUND:
			/* param1: entryPoint */
			return pcreateBackground((void *) param1);


		case SYS_CALL_END_PROCESS:
			/* param1: pid */
			return pkill(param1);

		case SYS_CALL_CREATE_THREAD:
			/* parm1: pid, param2: entryPoint */
			return tcreate(param1, (void *) param2);

		case SYS_CALL_END_THREAD:
			/* param1: pid, param2: pthread */
			return tkill(param1, param2);

		case SYS_CALL_CREATE_MUTEX:
			/* no params */
			return createMutexSysCall();

		case SYS_CALL_END_MUTEX:
			/* param1: id */
			return endMutexSysCall(param1);

		case SYS_CALL_UP_MUTEX:
			/* param1: id */
			return upMutexSysCall(param1);

		case SYS_CALL_DOWN_MUTEX:
			/* param1: id */			
			return downMutexSysCall(param1);

		case SYS_CALL_CURRENT_PID:
			/* no params */
			return getCurrentPidSysCall();
	}

	
	return 0;
}