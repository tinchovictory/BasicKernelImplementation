#include "memoryManager.h"

void * initMem;
int * usedMap;
int blocksAmount;

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
	uint64_t auxMem = 0;
	blocksAmount = -1;

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
	int i, size = MAP_SIZE(blocksAmount); // ints required to represent the state of each block.

	usedMap = (int *) (initMem + blocksAmount*PAGE_SIZE);

	for(i = 0 ; i < size; i++) { // Reset map.
        usedMap[i] = 0;
	}

	return;
}

/***********************************************************
 * Allocation Functions									   *
 ***********************************************************/

void * allocate (uint64_t request) {
	int blocksRequest = DIV_CEIL(request,PAGE_SIZE);
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

	return initMem + i*PAGE_SIZE;
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

int deallocate (void * startingPosition, uint64_t size) {
	int blocks = DIV_CEIL(size,PAGE_SIZE);
	int startingBlock = (startingPosition - initMem) / PAGE_SIZE;
	int i, limit = startingBlock + blocks;

	if (limit > blocksAmount) {
		return 0;
	}

	for (i = startingBlock ; i < limit ; i++) {
		resetBit (usedMap,i);
	}

	return 1;
}