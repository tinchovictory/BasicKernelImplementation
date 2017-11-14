#include "videoDriver.h"
#include <stdint.h>

static char * screen = (char *)0xB8000;
static char * currentPosition = (char*)0xB8000;

void clearScreen() {
	clear(screen);
	currentPosition = screen;
}

// Separated from resetScreen for reusability purposes in screenLoader.c
void clear(char * screenToClear) {
	int i;
	for(i = 0; i < WINDOW_SIZE; i++){
		if(i%2 == 1){
			screenToClear[i] = DEFAULT_COLOR;
		}else{
			//screenToClear[i] = ' ';
			screenToClear[i] = ' ';
		}
	}
}

void setScreen(char * myScreen, char * myCurrentPos) {
	int cursorDistance = (int)(myCurrentPos - myScreen);
	int i = 0;

	for(i = 0; i < WINDOW_SIZE; i++){
		screen[i] = myScreen[i];
	}
	currentPosition = screen + cursorDistance;
}

char * backupContent(char * backup) {
	int cursorDistance = (int)(currentPosition - screen);
	int i = 0;
	for(i = 0 ; i < WINDOW_SIZE ; i++){
		backup[i] = screen[i];
	}
	return backup + cursorDistance; // backup Cursor
}

void moveScreenUp(char * myScreen, char ** myCurrentPos){
	int i;
	for(i = 0; i< WINDOW_WIDTH*2 * (WINDOW_HEIGHT-1); i++){
		myScreen[i] = myScreen[i+WINDOW_WIDTH*2];
	}
	for(i = 0; i < WINDOW_WIDTH*2; i+=2){
		*(myScreen + END_OF_SCREEN - WINDOW_WIDTH*2 + i) = ' ';
		*(myScreen + END_OF_SCREEN - WINDOW_WIDTH*2 + i + 1) = DEFAULT_COLOR;
	}
	*myCurrentPos = myScreen + END_OF_SCREEN - WINDOW_WIDTH*2;
}

void checkEndOfScreen(char * myScreen, char ** myCurrentPos){
	if(*myCurrentPos >= myScreen + END_OF_SCREEN){
		moveScreenUp(myScreen,myCurrentPos);
	}
}

void print(const char * string){
	while(*string){
		*(currentPosition++) = *(string++);
		*(currentPosition++) = DEFAULT_COLOR;
		checkEndOfScreen(screen,&currentPosition);
	}
}

void printDec(int number){
	int length = intLength(number);
	char character;
	int divisor = 1;

	while (length - 1 > 0) {
		divisor *= 10;
	}

	while (divisor) {
		character = (number % 10) + '0';
		printCharacters(character);
		number /= 10;
		divisor /= 10;
	}
}

void newLineVid() {
	newLine(screen,currentPosition);
};

void newLine(char * myScreen, char ** myCurrentPos){
	*myCurrentPos +=  (myScreen - *myCurrentPos) % (WINDOW_WIDTH*2) + WINDOW_WIDTH*2;
	checkEndOfScreen(myScreen,myCurrentPos);
}

void backSpace(char * myScreen, char ** myCurrentPos){
	if(*myCurrentPos == myScreen){
		return;
	}
	*myCurrentPos -= 2;
	*(*myCurrentPos) = ' ';
}

void printTab(char ** myCurrentPos){
	*myCurrentPos += 8; 
}

void printCharacters(const char character){
	printCharactersInner(character,screen,&currentPosition);
}

void printCharactersInner(const char character, char * myScreen, char ** myCurrentPos){
	// Check for special characters.
	if(character == '\n'){
		newLine(myScreen,myCurrentPos);
	}else if(character == '\b'){
		backSpace(myScreen,myCurrentPos);
	}else if(character == '\t'){
		printTab(myCurrentPos);
	}else{
		*((*myCurrentPos)++) = character;
		*((*myCurrentPos)++) = DEFAULT_COLOR;
	}
	checkEndOfScreen(myScreen,myCurrentPos);
}