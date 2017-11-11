#ifndef __VIDEO_DRIVER__
	#define __VIDEO_DRIVER__

	#define WINDOW_WIDTH 80
	#define WINDOW_HEIGHT 25
	#define WINDOW_SIZE WINDOW_WIDTH * WINDOW_HEIGHT * 2
	#define DEFAULT_COLOR 0x0F
	#define END_OF_SCREEN WINDOW_WIDTH*2 * WINDOW_HEIGHT

	#define NULL ((void *)0)

	void clearScreen();

	void moveScreenUp();

	void checkEndOfScreen();

	void print( const char * string );

	void printCharacters(const char character);

	void newLine();

	void backSpace();

	void printTab();

	// New functions for Operating Systems project

	void setScreen(char * data);

	void transferContent(char * backup);

	void clear(char * screenToClear);

#endif