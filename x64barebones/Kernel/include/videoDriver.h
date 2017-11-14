#ifndef __VIDEO_DRIVER__
	#define __VIDEO_DRIVER__

	#define WINDOW_WIDTH 80
	#define WINDOW_HEIGHT 25
	#define WINDOW_SIZE WINDOW_WIDTH * WINDOW_HEIGHT * 2
	#define DEFAULT_COLOR 0x0F
	#define END_OF_SCREEN WINDOW_WIDTH*2 * WINDOW_HEIGHT
	#define TABSIZE 12

	#define NULL ((void *)0)

	void clearScreen();

	void moveScreenUp(char * myScreen, char ** myCurrentPos);

	void checkEndOfScreen(char * myScreen, char ** myCurrentPos);

	void print(const char * string);

	void printDec(int number);

	void printCharacters(const char character);
	void printCharactersInner(const char character, char * myScreen, char ** myCurrentPos);

	void newLine();

	void myNewLine(char * myScreen, char ** myCurrentPos);

	void backSpace(char * myScreen, char ** myCurrentPos);

	void myPrintTab(char ** myCurrentPos);

	void printTab();

	void setScreen(char * myScreen, char * myCurrentPos);

	char * backupContent(char * backup);

	void clear(char * screenToClear);

#endif