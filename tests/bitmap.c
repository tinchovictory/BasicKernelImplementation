#include "bitmap.h"

void setBit (int * bitMap, int position) {
	int * sector = SECTOR_ADDR(bitMap,position);

	*sector |= SET_MASK(position);

	return;
}

void resetBit (int * bitMap, int position) {
	int * sector = SECTOR_ADDR(bitMap,position);

	*sector &= RESET_MASK(position);

	return;
}