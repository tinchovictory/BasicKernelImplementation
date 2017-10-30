#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "memoryManager.h"

void ceilTest();

void bitMapTest();

void managerTest();

int testMemAllocatorInit(void * initPosition, void * endPosition);

void testMapInit();

extern void * initMem;
extern int * usedMap;
extern int blocksAmount;

int main (int argc, char *argv[]) {

	ceilTest(); // Test of my CEIL macro.

	bitMapTest();

	managerTest();

	return 0;
}

void ceilTest() {
	int a = 3;
	int b = 2;

	assert(CEIL(a,b) == 2);

	a = 2;

	assert(CEIL(a,b) == 1);

	a = -3;

	assert(CEIL(a,b) == -1);

	uint64_t a2 = 30000;
	uint64_t b2 = 20000;

	assert(CEIL(a2,b2) == 2);

	a2 = 20000;

	assert(CEIL(a2,b2) == 1);

	return;
}

void bitMapTest() {

	uint64_t pageSize = 0x1000;

	int bits = 1138;

	int mapSize = MAP_SIZE(bits);

	assert(  bits < mapSize*sizeof(int)*BYTE_SIZE ); // I check if the map got the right size.
	assert(  bits > (mapSize-1)*sizeof(int)*BYTE_SIZE );

	int * map = (int *) calloc(mapSize,sizeof(int));

	int position;

	for (position = 0; position < bits; position++) {
		// Initial Test to the IS_AVAILABLE macro. Map should be empty (fully available).
		assert( IS_AVAILABLE(map,position) == 1 );
	}

	for (position = 0; position < bits; position++) {
		setBit(map,position);
	}

	for (position = 0; position < bits; position++) {
		assert( IS_AVAILABLE(map,position) == 0 );
	}

	for (position = 0; position < bits; position++) {
		resetBit(map,position);
	}

	for (position = 0; position < bits; position++) {
		assert( IS_AVAILABLE(map,position) == 1 );
	}

	int testPosition = 4;

	setBit(map,testPosition);

	for (position = 0; position < bits; position++) {
		if (position == testPosition) {
			assert( IS_AVAILABLE(map,position) == 0 );
		} else {
			assert( IS_AVAILABLE(map,position) == 1 );
		}
	}

	resetBit(map,testPosition);

	for (position = 0; position < bits; position++) {
		assert( IS_AVAILABLE(map,position) == 1 );
	}
}

void managerTest() {
	int testPageAmount = 5;
	void * initPosition = (void*)0x300000;
	void * endPosition = initPosition + testPageAmount*PAGE_SIZE;

	testMemAllocatorInit(initPosition,endPosition);

	// I require extra space to save the map and its pointer.
	assert(blocksAmount == (testPageAmount -1));

	int mapSize = MAP_SIZE(testPageAmount);

	endPosition = initPosition + testPageAmount*PAGE_SIZE + sizeof(uint64_t) + mapSize;

	testMemAllocatorInit(initPosition,endPosition);

	assert(blocksAmount == testPageAmount);

	uint64_t request = 2*PAGE_SIZE;
	void * savePosition = allocate(request);

	assert(savePosition == initPosition);

	assert( IS_AVAILABLE(usedMap,1) == 0 ); // Last reserved page.
	assert( IS_AVAILABLE(usedMap,2) == 1 );

	request = 4*PAGE_SIZE;
	savePosition = allocate(request);

	// I need 4 blocks but I have 3 available.
	assert(savePosition == NULL);

	request = 3*PAGE_SIZE;
	savePosition = allocate(request);

	assert(savePosition == (initPosition + 2*PAGE_SIZE));

	int i;

	for (i = 0 ; i < blocksAmount ; i++) { // All pages used.
		assert( IS_AVAILABLE(usedMap,i) == 0 );
	}

	int deallocResult = deallocate (initPosition,6*PAGE_SIZE);

	assert(deallocResult == 0); // Not enough pages.

	deallocResult = deallocate (initPosition,5*PAGE_SIZE);

	assert(deallocResult == 1);

	for (i = 0 ; i < blocksAmount ; i++) {
		assert( IS_AVAILABLE(usedMap,i) == 1 );
	}

	request = 3*PAGE_SIZE;
	savePosition = allocate(request);

	// I need 4 blocks but I have 3 available.
	assert(savePosition == initPosition);

	request = PAGE_SIZE;
	savePosition = allocate(request);

	assert(savePosition == (initPosition + 3*PAGE_SIZE));
}

// mapInit from memAllocatorInit replaced to use calloc.
int testMemAllocatorInit (void * initPosition, void * endPosition) {
	uint64_t totalMem = endPosition - initPosition;

	if (totalMem < PAGE_SIZE) { // There is not enough memory for the memory allocator.
		return 0;
	}

	getBlocksAmount(totalMem);

	initMem = initPosition;

	testMapInit();

	return 1;
}

void testMapInit () {
	int size = MAP_SIZE(blocksAmount); // ints required to represent the state of each block.

	usedMap = (int *) malloc((size_t)(initMem + blocksAmount*PAGE_SIZE));

	int i;

	for(i = 0 ; i < size; i++) { // Reset map.
        *(usedMap + i) = 0;
	}

	return;
}