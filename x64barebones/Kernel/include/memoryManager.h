#ifndef __MEMORYMANAGER
#define __MEMORYMANAGER

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


#include "myMath.h"
#include "bitmap.h"

#define PAGE_SIZE 0x1000

// Init Functions

int memAllocatorInit (void * initPosition, void * endPosition);

void getBlocksAmount (uint64_t totalMem);

int enoughSpace (int remainingMem);

void mapInit ();

// Allocation Functions

void * allocate (uint64_t request);

void reserve (int blocksRequest, int startingBlock);

// Deallocation Function

int deallocate (void * startingPosition, uint64_t size);

#endif
