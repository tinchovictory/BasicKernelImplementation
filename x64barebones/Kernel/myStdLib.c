#include "myStdLib.h"

int intLength(int number);

void  itoa ( int value, char * str) {
	int length = intLength(value);

	if (length == 0) {
		str[0] = '0';
		str[1] = 0;
	}

	str[length] = 0;

	while (length) {
		str[length-1] = (number % 10) + '0';
		number /= 10;
	}
};

int intLength(int number) {
	int length = 0;

	while (number) {
		number /= 10;
		length++;
	}
	return length;
}