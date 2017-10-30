#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "memoryManager.h"

static void * initMem;

static int * usedMap;

static int blocksAmount = 0;

/***********************************************************
 * Start of Memory Allocator Initializer				   *
 ***********************************************************/

int memAllocatorInit (void * initPosition, void * endPosition) {
	uint64_t totalMem = endPosition - initPosition;

	if (totalMem < PAGE_SIZE) { // There is not enough memory for the memory allocator.
		return 0;
	}

	getBlocksAmount(totalMem);

	initMem = initPosition;

	mapInit();

	return 1;
}

void getBlocksAmount (uint64_t totalMem) {
	blocksAmount = -1;
	uint64_t auxMem = 0;

	do {
		auxMem += PAGE_SIZE;
		blocksAmount++;
	} while (auxMem <= totalMem);

	auxMem -= PAGE_SIZE;

	int remainingMem = (int) (totalMem - auxMem);

	while (!enoughSpace(remainingMem)) {
		blocksAmount--;
		remainingMem += PAGE_SIZE;
	}

	return;
}

int enoughSpace (int remainingMem) {
	int spaceForMap = sizeof(uint64_t); // The pointer
	spaceForMap += MAP_SIZE(blocksAmount); // The map

	return spaceForMap <= remainingMem;
}

void mapInit () {
	int size = MAP_SIZE(blocksAmount); // ints required to represent the state of each block.

	usedMap = (int *) (initMem + blocksAmount*PAGE_SIZE);

	int i;

	for(i = 0 ; i < size; i++) { // Reset map.
        *(usedMap + i) = 0;
	}

	return;
}

/***********************************************************
 * End of Memory Allocator Initializer					   *
 ***********************************************************/

int * getMap () {
	return usedMap;
}

/***********************************************************
 * Allocation Functions									   *
 ***********************************************************/

void * allocate (uint64_t request) {
	int blocksRequest = CEIL(request,PAGE_SIZE);
	int i, j, allocFound = 0;
	// After lastIndex, it is mathematically impossible to find a candidate.
	int lastIndex = blocksAmount - blocksRequest;

	for (i = 0 ; i <= lastIndex && !allocFound ;) {
		allocFound = 1;

		for (j = 0 ; j < blocksRequest && allocFound ; j++) {
			allocFound = IS_AVAILABLE(usedMap,i+j);
		}

		i += j;
	}
	// If the request is bigger than the available space (negative lastIndex)
	// or if a large enough space was not found.
	if (!allocFound) {
		return NULL;
	}

	i -= j; // Move back to the starting position of the candidate space.

	reserve(blocksRequest,i);

	void * startingMem = initMem + i*PAGE_SIZE;

	return startingMem;
}

void reserve (int blocksRequest, int startingBlock) {
	int i, limit = startingBlock + blocksRequest;

	for (i = startingBlock ; i < limit ; i++) {
		setBit (usedMap,i);
	}
	return;
}

/***********************************************************
 * Deallocation Functions								   *
 ***********************************************************/

void deallocate (void * startingPosition, uint64_t size) {
	int blocks = CEIL(size,PAGE_SIZE);
	int startingBlock = (startingPosition - initMem) / PAGE_SIZE;
	int i, limit = startingBlock + blocks;

	for (i = startingBlock ; i < limit ; i++) {
		resetBit (usedMap,i);
	}
	return;
}