#include <stdio.h>
#include <string.h>
#include <systemCalls.h>
#include <philosophers.h>

void thread() {
	while(1) {
		int j = 0;
		while(j < 20000000) {
			j++;
		}
		printf("soy el thread\n");
	}
}

void test() {
	while(1) {
		char c;
		while((c = getchar()) != '\n') {
			//putchar(c);		
		}
		char * array = (char *) malloc(sizeof(char) * 5);
		array[0] = 'h';
		array[1] = 'o';
		array[2] = 0;
		putchar(c);
		printf("%s\n", array);
		int pthred = tcreate(getCurrentPid(),thread);
		int i = 0;
		while(1) {
			int j = 0;
			while(j < 20000000) {
				j++;
			}
			printf("soy el thread 0\n");
			i++;
			if(i == 15) {
				printf("Mato al thread\n");
				tkill(getCurrentPid(), pthred);
			}
		}
	}
}
int mutex = -1;
void testMutex2() {
	while(mutex == -1);

	printf("Intento entrar\n");
	mutexDown(mutex);
	printf("Entre\n");
	int j = 0;
		while(j<100000000) {
			j++;
		}
	printf("Voy a salir\n");
	mutexUp(mutex);
	printf("Ya sali\n");


	while(1);
}

void testMutex() {
	tcreate(getCurrentPid(), testMutex2);
	//pcreate(testMutex2);
	mutex = createMutex();
	mutexDown(mutex);
	printf("Bloqueo\n");
	
	int j = 0;
	while(j<100000000) {
		j++;
	}

	mutexUp(mutex);
	printf("Lo libere\n");


	while(1);
}

/* Desde aca */

void run(void * entryPoint) {
	int pid = pcreate(entryPoint);
	//foreground
	//killprocess
}

void processComand(char * buffer){
	if (!strcmp(buffer,"help")){
		printf("  echo : print on screen\n");
		printf("  hola : saludo de la consola\n");
		printf("  2048game : Juego 2048\n");
		printf("  chat : enter to chat\n");
		printf("  clear : clear screen\n");
		printf("  ps : prints processes with attributes\n");
		printf("  philosophers : ​prints solution to philosophers problem\n");
		printf("  prodcons : prints solution to producer-consumer problem\n");
	}
	else if(startsWith("echo ",buffer)){
		puts(buffer+5);
	}
	else if(!strcmp(buffer,"hola")){
		puts("  Hola! Mi nombre es NetSky");
	}else if(!strcmp(buffer,"2048game")){
		clearScreen();
		printf("Run 2048\n");
		//game2048();
	}
	else if(!strcmp("clear",buffer)){
		clearScreen();
	}else if(startsWith("chat",buffer)){
		clearScreen();
		printf("Run my Chat\n");
		//myChat();
	}else if(!strcmp("ps",buffer)){
		ps();
	}else if(!strcmp("philosophers",buffer)){
		//run(philosophers);
	}else if(startsWith("prodcons",buffer)){
		//TODO
	}else if(!strcmp(buffer,"test")) {
		run(test);
	}else if(!strcmp(buffer,"mutex")){
		run(testMutex);
	}else{
		puts("  Command not found - help for instructions");
	}
}

void shell(){

	char buffer[100];
	int i=0;
	while(1){
		char c;
		printf("$> ");
		while ((c=getchar())!= '\n'){
			if(c != '\b'){
				buffer[i++]=c;
				putchar(c);
			}else if (i>0){
				i--;
				putchar(c);
			}	
		}
		putchar(c);
		buffer[i]=0;
		processComand(buffer);
		i=0;
	}
	

}