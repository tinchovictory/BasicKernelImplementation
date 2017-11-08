#include <stdint.h>
#define WINDOW_WIDTH 80
#define WINDOW_HEIGHT 25
#define DEFAULT_COLOR 0x0F
#define END_OF_SCREEN WINDOW_WIDTH*2 * WINDOW_HEIGHT

static char * screen = (char *)0xB8000;
static char * currentPosition = (char*)0xB8000;

void clearScreen(){
	for(int i = 0; i < WINDOW_WIDTH*2 * WINDOW_HEIGHT; i++){
		if(i%2 == 1){
			screen[i] = DEFAULT_COLOR;
		}else{
			screen[i] = ' ';
		}
	}
	currentPosition = screen;
}

void moveScreenUp(){
	for(int i = 0; i< WINDOW_WIDTH*2 * (WINDOW_HEIGHT-1); i++){
		screen[i] = screen[i+WINDOW_WIDTH*2];
	}
	for(int i = 0; i < WINDOW_WIDTH*2; i+=2){
		*(screen + END_OF_SCREEN - WINDOW_WIDTH*2 + i) = ' ';
		*(screen + END_OF_SCREEN - WINDOW_WIDTH*2 + i + 1) = DEFAULT_COLOR;
	}
	currentPosition = screen + END_OF_SCREEN - WINDOW_WIDTH*2;
}

void checkEndOfScreen(){
	if(currentPosition >= screen + END_OF_SCREEN){
		moveScreenUp();
	}
}

void print(const char * string){
	while(*string){
		*(currentPosition++) = *(string++);
		*(currentPosition++) = DEFAULT_COLOR;
		checkEndOfScreen();
	}
}

void newLine(){
	currentPosition +=  (screen - currentPosition) % (WINDOW_WIDTH*2) + WINDOW_WIDTH*2;
	checkEndOfScreen();
}

void backSpace(){
	if(currentPosition == screen){
		return;
	}
	currentPosition-=2;
	*currentPosition = ' ';
}

void printTab(){
	currentPosition += 8; 
}

void printCharacters(const char character){
	//analizo caracteres especiales
	if(character == '\n'){
		newLine();
	}else if(character == '\b'){
		backSpace();
	}else if(character == '\t'){
		printTab();
	}else{
		*(currentPosition++) = character;
		*(currentPosition++) = DEFAULT_COLOR;
	}
	checkEndOfScreen();
}
/*
char * strnum(int value, char * str, int base) {
	int i = 0;
	char * p1, * p2;
	int rem;
 
    /* Handle 0 explicitely, otherwise empty string is printed for 0 *//*
    if (value == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    // Process individual digits
    while (value != 0)
    {
        rem = value % base;
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
        value = value/base;
    }
 	// Append string terminator
    str[i] = '\0';
 
    // Reverse the string
    p1 = str;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
    return str;
}*/
