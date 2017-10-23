#ifndef _SYSTEM_CALLS_
#define _SYSTEM_CALLS_

#include <stdint.h>

#define SYS_CALL_READ 1
#define SYS_CALL_WRITE 2
#define SYS_CALL_CLEAR_SCREEN 3
#define SYS_CALL_MEMORY 4

#define MEMORY_ASIGN_CODE 0
#define MEMORY_FREE_CODE 1

#define STANDARD_IO_FD 1
#define ETHERNET_FD 2

uint64_t systemCall(uint64_t systemCallNumber, uint64_t fileDescriptor, void * buf, uint64_t nBytes); // asm function

#endif
