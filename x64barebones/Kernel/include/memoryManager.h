#ifndef __MEMORYMANAGER
#define __MEMORYMANAGER

#include <stdio.h>
#include <stdint.h>

#include "bitmap.h"

#define PAGE_SIZE 0x1000

int memAllocatorInit (void * initPosition, void * endPosition);

void getBlocksAmount (uint64_t totalMem);

int enoughSpace (int remainingMem);

void * allocate (uint64_t request);

void reserve (int blocksRequest, int startingBlock);

void deallocate (void * startingPosition, uint64_t size);

#endif
