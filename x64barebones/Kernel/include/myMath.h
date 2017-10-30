#ifndef __MYMATH
#define __MYMATH

// My Ceil is different from that defined in math.h
// This will return a int instead of a double.
#define CEIL(x,y) ((x) > 0 ? POSITIVE_CEIL(x,y) : NEGATIVE_CEIL(x,y) )
#define POSITIVE_CEIL(x,y) ((x + (x)%(y))/(y))
#define NEGATIVE_CEIL(x,y) ((x)/(y))

#endif