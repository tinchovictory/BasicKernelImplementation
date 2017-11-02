#ifndef __MYMATH
#define __MYMATH

// My Ceil is different from that defined in math.h
// This will return an int instead of a double
// and take a numerator and a denominator as parameters to work for a division.
#define DIV_CEIL(x,y) ( (y) > 0 ? DIV_CEIL_FIXED(x,y) : DIV_CEIL_FIXED(-(x),-(y)) )

#define DIV_CEIL_FIXED(x,y) ( (x) > 0 ? POSITIVE_DIV_CEIL(x,y) : NEGATIVE_DIV_CEIL(x,y) )

#define POSITIVE_DIV_CEIL(x,y) (POSITIVE_DIV_CEIL_NUM(x,y)/(y))

#define POSITIVE_DIV_CEIL_NUM(x,y) ((x) + ((y) - ((x)%(y)))%(y))

#define NEGATIVE_DIV_CEIL(x,y) ((x)/(y))

#endif