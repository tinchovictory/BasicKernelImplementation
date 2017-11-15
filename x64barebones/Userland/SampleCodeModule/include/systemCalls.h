#ifndef _SYSTEM_CALLS_USER_
#define _SYSTEM_CALLS_USER_

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
#define SYS_CALL_SEND 21
#define SYS_CALL_RECEIVE 22
#define SYS_CALL_CREATE_PIPE 23

#define STANDARD_IO_FD 1
#define ETHERNET_FD 2

uint64_t systemCall(uint64_t systemCallNumber, uint64_t param1, uint64_t param2, uint64_t param3); // asm function
uint64_t yield();

/* Returns the number of bytes that were read. If value is negative, then the system call returned an error. */
int read(int fildes, void *buf, unsigned int nbytes);


/* Returns the number of bytes that were written. If value is negative, then the system call returned an error. */
int write(int fildes, void *buf, unsigned int nbytes);


void clearScreen();

void * malloc(int bytes);

void free(void * memPosition);

void ps();

int pcreate(void * exec, void * entryPoint, char * name);

int pcreateBackground(void * exec, void * entryPoint, char * name);

void exit();

void pkill(int pid);

int tcreate(int pid, void * exec, void * entryPoint);

void tkill(int pid, int pthread);

int createMutex();

void endMutex(int id);

void mutexUp(int id);

void mutexDown(int id);

int getCurrentPid();

int createSemaphore(int start);

void endSemaphore(int id);

void semaphoreUp(int id);

void semaphoreDown(int id);

char * createPipe(int fromPid, int toPid);

void send(char * name, char * buff);

void receive(char * name, char * buff);

#endif