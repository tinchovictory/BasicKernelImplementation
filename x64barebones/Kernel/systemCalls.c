#include "systemCalls.h"
#include <videoDriver.h>
#include <keyBoardDriver.h>
#include <RTL8139.h>
#include <scheduler.h>
#include "screenLoader.h"
#include "naiveConsole.h"

#define SYS_CALL_READ 1
#define SYS_CALL_WRITE 2
#define SYS_CALL_CLEAR_SCREEN 3
#define SYS_CALL_MEMORY 4
#define SYS_CALL_PS 5

#define SYS_CALL_CREATE_PROCESS 7
#define SYS_CALL_END_PROCESS 8
#define SYS_CALL_LS_PROCESS 9

#define SYS_CALL_CREATE_THREAD 10
#define SYS_CALL_END_THREAD 11

#define MEMORY_ASIGN_CODE 0
#define MEMORY_FREE_CODE 1

#define STANDARD_IO_FD 1
#define ETHERNET_FD 2

//static void * memory = (void *)0x900000;

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
			blockProcess(getCurrentPid());	
		}
		return readBytes;
	}else if(fileDescriptor == ETHERNET_FD){
		return getMsg((ethMsg *) buf);
	}
	return -1;
}

int blockIfNotOnFocus(){
	if (!isCurrentProcessOnFocus()) {
		blockProcess(getCurrentPid());
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
	//ncPrint("About to write in place other than video screen.");ncNewline();

	char * myBuf = (char *) buf;
	char * myScreen = getCurrentScreen();
	char * myScreenPos = getCurrentScreenPosition();
	int i;

	//ncPrintHex(myScreenPos);ncNewline();

	for(i = 0; i < nBytes && myBuf[i] != 0; i++){
		printCharactersInner(myBuf[i],myScreen,&myScreenPos);
	}

	setCurrentScreenPosition(myScreenPos);

	//ncPrintHex(myScreenPos);ncPrint(", i:");ncPrintDec(i);ncNewline();

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
	return addProcess(entryPoint);
}

uint64_t tcreate(uint64_t pid, void * entryPoint){
	return addThreadToProcess(pid, entryPoint);

}

void pkill(uint64_t pid){
	removeProcess(pid);
}

void tkill(uint64_t pid, uint64_t pthread){
	removeThread(pid, pthread);
}

void ls(uint64_t pid){
	printProcessPID(pid);
}

void ps(){
	printAllProcess();
}

uint64_t blockProcessWithPid(uint64_t pid) {

	return 1;
}

uint64_t yield(uint64_t pid) {

	return 1;
}


uint64_t systemCall(uint64_t systemCallNumber, uint64_t fileDescriptor, void * buf, uint64_t nBytes){
	if(systemCallNumber == SYS_CALL_READ){
		return read(fileDescriptor, buf, nBytes);
	}else if(systemCallNumber == SYS_CALL_WRITE){
		return write(fileDescriptor, buf, nBytes);
	}else if(systemCallNumber == SYS_CALL_CLEAR_SCREEN){
		return clearScreenSys();
	} //else if(systemCallNumber == SYS_CALL_MEMORY){
		//return memoryManagement(fileDescriptor, nBytes);
	//}/*else if(systemCallNumber == SYS_CALL_PS){
	//	return ps();
	//}*/
	else if(systemCallNumber == SYS_CALL_CREATE_PROCESS){
		return pcreate(buf);
	}else if(systemCallNumber == SYS_CALL_END_PROCESS){
		pkill(fileDescriptor);

	}else if(systemCallNumber == SYS_CALL_LS_PROCESS){
		ls(fileDescriptor);
	}else if(systemCallNumber == SYS_CALL_CREATE_THREAD){
		return tcreate(fileDescriptor,buf);

	}else if(systemCallNumber == SYS_CALL_END_THREAD){
		tkill(fileDescriptor,nBytes);

	}else if(systemCallNumber == SYS_CALL_PS){
		ps();
	}
	return 0;
}