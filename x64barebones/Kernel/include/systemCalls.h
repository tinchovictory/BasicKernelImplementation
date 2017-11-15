#ifndef _SYSTEM_CALLS_
#define _SYSTEM_CALLS_

#include <stdint.h>

#define SYS_CALL_READ 1
#define SYS_CALL_WRITE 2
#define SYS_CALL_CLEAR_SCREEN 3
#define SYS_CALL_MEMORY_ASSIGN 4
#define SYS_CALL_MEMORY_FREE 5
#define SYS_CALL_PS 6
#define SYS_CALL_CREATE_PROCESS 7
#define SYS_CALL_END_PROCESS 8
#define SYS_CALL_CREATE_THREAD 9
#define SYS_CALL_END_THREAD 10
#define SYS_CALL_CREATE_MUTEX 11
#define SYS_CALL_END_MUTEX 12
#define SYS_CALL_UP_MUTEX 13
#define SYS_CALL_DOWN_MUTEX 14
#define SYS_CALL_CURRENT_PID 15
#define SYS_CALL_CREATE_PROCESS_BACKGROUND 16
#define SYS_CALL_CREATE_SEMAPHORE 17
#define SYS_CALL_END_SEMAPHORE 18
#define SYS_CALL_UP_SEMAPHORE 19
#define SYS_CALL_DOWN_SEMAPHORE 20

//#define MEMORY_ASIGN_CODE 0
//#define MEMORY_FREE_CODE 1

#define STANDARD_IO_FD 1
#define ETHERNET_FD 2


uint64_t clearScreenSys();

uint64_t read(uint64_t fileDescriptor, void * buf, uint64_t nBytes);

int blockIfNotOnFocus();

uint64_t write(uint64_t fileDescriptor, void * buf, uint64_t nBytes);

uint64_t pcreate(void * exec, void * entryPoint, char * name);

uint64_t pcreateBackground(void * exec, void * entryPoint, char * name);

uint64_t tcreate(uint64_t pid, void * exec, void * entryPoint);

uint64_t pkill(uint64_t pid);

uint64_t tkill(uint64_t pid, uint64_t pthread);

uint64_t ps();

void * mallocSysCall(uint64_t bytes);

uint64_t freeSysCall(void * memoryPosition);

uint64_t createMutexSysCall();

uint64_t endMutexSysCall(uint64_t id);

uint64_t upMutexSysCall(uint64_t id);

uint64_t downMutexSysCall(uint64_t id);

uint64_t getCurrentPidSysCall();

uint64_t createSemaphoreSysCall(uint64_t start);

uint64_t endSemaphoreSysCall(uint64_t id);

uint64_t upSemaphoreSysCall(uint64_t id);

uint64_t downSemaphoreSysCall(uint64_t id);



uint64_t systemCall(uint64_t systemCallNumber, uint64_t param1, uint64_t param2, uint64_t param3); // asm function

#endif