#include "myStdLib.h"

int itoa ( int value, char * str) {
	int length = intLength(value);
	int aux = length;

	if (length == 0) {
		str[0] = '0';
		str[1] = 0;
	}

	str[length] = 0;

	while (aux) {
		str[aux-1] = (value % 10) + '0';
		value /= 10;
		aux--;
	}
	return length;
};

int intLength(int number) {
	int length = 0;

	while (number) {
		number /= 10;
		length++;
	}
	return length;
}