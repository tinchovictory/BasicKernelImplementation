#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "memoryManager.h"

void ceilTest();

void bitMapTest();

int main (int argc, char *argv[]) {

	ceilTest(); // Test of my CEIL macro.

	bitMapTest();

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

	assert (  bits < mapSize*sizeof(int)*BYTE_SIZE ); // I check if the map got the right size.
	assert (  bits > (mapSize-1)*sizeof(int)*BYTE_SIZE );

	int * map = (int *) calloc(mapSize,sizeof(int));

	int position;
	int * sector;

	for (position = 0; position < bits; position++) {
		// Initial Test to the IS_AVAILABLE macro. Map should be empty (fully available).
		assert ( IS_AVAILABLE(map,position) == 1 );
	}

	for (position = 0; position < bits; position++) {
		sector = SECTOR_ADDR(map,position);
		*sector = *sector | SET_MASK(position);
	}

	for (position = 0; position < bits; position++) {
		assert ( IS_AVAILABLE(map,position) == 0 );
	}

	for (position = 0; position < bits; position++) {
		sector = SECTOR_ADDR(map,position);
		*sector = *sector & RESET_MASK(position);
	}

	for (position = 0; position < bits; position++) {
		assert ( IS_AVAILABLE(map,position) == 1 );
	}

	int testPosition = 4;

	sector = SECTOR_ADDR(map,testPosition);
	*sector = *sector | SET_MASK(testPosition);

	for (position = 0; position < bits; position++) {
		if (position == testPosition) {
			assert ( IS_AVAILABLE(map,position) == 0 );
		} else {
			assert ( IS_AVAILABLE(map,position) == 1 );
		}
	}

	sector = SECTOR_ADDR(map,testPosition);
	*sector = *sector & RESET_MASK(testPosition);

	for (position = 0; position < bits; position++) {
		assert ( IS_AVAILABLE(map,position) == 1 );
	}
}