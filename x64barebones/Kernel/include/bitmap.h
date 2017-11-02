#ifndef __BITMAP
#define __BITMAP

#include "myMath.h"

// The bitmap that we are using is meant to be used with a pointer to int.

// The size of a bit
#define BYTE_SIZE 8

#define INT_BITS ( sizeof(int) * BYTE_SIZE )

#define MAP_SIZE(bits) DIV_CEIL(bits,INT_BITS)

#define IS_AVAILABLE(bitMap,position) ( IS_AVAILABLE_INNER( SECTOR(bitMap,position) , SECTOR_POSITION(position) ) )

#define IS_AVAILABLE_INNER(sector,position) ( GET_BIT(sector,position) == 0 )

// The bitwise operator "">>"" moves the bit that I care about to the right. Then, I use a mask
// to put everything else to 0.
#define GET_BIT(sector,position) ( ( (sector) >> (position) ) & 1 )

#define SECTOR(bitMap,position) (bitMap)[(position) / INT_BITS]

#define SECTOR_POSITION(position) ( (position) % INT_BITS )

#define SECTOR_ADDR(bitMap,position) &SECTOR(bitMap,position)

#define SET_MASK(position) ( 1 << SECTOR_POSITION(position) )

#define RESET_MASK(position) ~SET_MASK(position)

void setBit (int * bitMap, int position);

void resetBit (int * bitMap, int position);

#endif