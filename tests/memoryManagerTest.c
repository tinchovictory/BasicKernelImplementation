#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "memoryManager.h"

#define AVAILABLE 1
#define UNAVAILABLE 0

void ceilTest();
void ceilTest1();
void ceilTest2();

void bitMapTest();
void mapAvailabilityTest(int expectedState, int bits, int * map);

void managerTest();
void managerUseTest();
void allocTest(uint64_t request, void * expectedDir);

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
	
	ceilTest1();
	
	ceilTest2();

	return;
}

void ceilTest1() {

	int num = 3;
	int den = 2;

	assert(DIV_CEIL(num,den) == 2);

	num = -3;
	den = -2;

	assert(DIV_CEIL(num,den) == 2);

	num = 2;
	den = 2;

	assert(DIV_CEIL(num,den) == 1);

	num = -3;

	assert(DIV_CEIL(num,den) == -1);

	num = -3;
	den = -2;

	assert(DIV_CEIL(num,den) == 2);

	num = 3;

	assert(DIV_CEIL(num,den) == -1);

	return;
}

void ceilTest2() {

	uint64_t a2 = 30000;
	uint64_t b2 = 20000;

	assert(DIV_CEIL(a2,b2) == 2);

	a2 = 20000;

	assert(DIV_CEIL(a2,b2) == 1);

	a2 = 10000;

	assert(DIV_CEIL(a2,b2) == 1);

	b2 = 3333;

	assert(DIV_CEIL(a2,b2) == 4);

	return;
}

void bitMapTest() {

	int bits = 1138;
	int position;
	int testPosition = 4;
	int mapSize = MAP_SIZE(bits);
	int * map = (int *) calloc(mapSize,sizeof(int));

	assert(  bits < mapSize*sizeof(int)*BYTE_SIZE ); // I check if the map got the right size.
	assert(  bits > (mapSize-1)*sizeof(int)*BYTE_SIZE );

	mapAvailabilityTest(AVAILABLE,bits,map);

	for (position = 0; position < bits; position++) {
		setBit(map,position); // I occupy each position of the map.
	}

	mapAvailabilityTest(UNAVAILABLE,bits,map);

	for (position = 0; position < bits; position++) {
		resetBit(map,position); // I make each position of the map available.
	}

	mapAvailabilityTest(AVAILABLE,bits,map);

	setBit(map,testPosition);

	for (position = 0; position < bits; position++) {
		if (position == testPosition) {
			assert( IS_AVAILABLE(map,position) == UNAVAILABLE );
		} else {
			assert( IS_AVAILABLE(map,position) == AVAILABLE );
		}
	}

	resetBit(map,testPosition);

	mapAvailabilityTest(AVAILABLE,bits,map);

	return;
}

void mapAvailabilityTest(int expectedState, int bits, int * map) {
	int position;

	for (position = 0; position < bits; position++) {
		assert( IS_AVAILABLE(map,position) == expectedState );
	}

	return;
}

void managerTest() {
	int testPageAmount = 5;
	void * initPosition = (void*)0x300000;
	void * endPosition = initPosition + testPageAmount*PAGE_SIZE;

	testMemAllocatorInit(initPosition,endPosition);

	// I require extra space to save the map and its pointer.
	assert(blocksAmount == (testPageAmount -1));

	endPosition = initPosition + testPageAmount*PAGE_SIZE + sizeof(uint64_t) + MAP_SIZE(testPageAmount);

	testMemAllocatorInit(initPosition,endPosition);

	assert(blocksAmount == testPageAmount);

	managerUseTest();

	return;
}

void managerUseTest() {
	uint64_t request = 2*PAGE_SIZE;
	void * savePosition = allocate(request);

	assert(savePosition == initMem);

	assert( IS_AVAILABLE(usedMap,1) == UNAVAILABLE ); // Last reserved page.
	assert( IS_AVAILABLE(usedMap,2) == AVAILABLE );

	allocTest(4*PAGE_SIZE,NULL); // I need 4 blocks but I have 3 available.
	allocTest(3*PAGE_SIZE,initMem + 2*PAGE_SIZE);

	mapAvailabilityTest(UNAVAILABLE,blocksAmount,usedMap);

	assert(deallocate (initMem,6*PAGE_SIZE) == 0); // Not enough pages.

	assert(deallocate (initMem,5*PAGE_SIZE) == 1); // Map should be all free now.

	mapAvailabilityTest(AVAILABLE,blocksAmount,usedMap);

	allocTest(3*PAGE_SIZE,initMem);
	allocTest(PAGE_SIZE,initMem + 3*PAGE_SIZE);

	return;
}

void allocTest(uint64_t request, void * expectedDir) {
	void * savePosition = allocate(request);

	assert(savePosition == expectedDir);

	return;
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
        usedMap[i] = 0;
	}

	return;
}