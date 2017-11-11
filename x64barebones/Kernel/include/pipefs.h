#ifndef __PIPEFS__
	#define __PIPEFS__
	
	#define NULL ((void *)0)

	#define NAME_SIZE 50
	#define PIPE_BUF_SIZE 4096

	typedef struct pipeNode {
		char name[NAME_SIZE];

		char * buffer;
		char * readPointer;
		char * writePointer;

		struct pipeNode * next;
	} pipeNode;


	char * do_pipe(int fromPid, int toPid);
	pipeNode * createPipe(int fromPid, int toPid);
	pipeNode * addPipeToList(pipeNode * list, pipeNode * pipe);
	void writeToPipe(char name[NAME_SIZE], char * buff);
	void readFromPipe(char name[NAME_SIZE], char * buff);
	char * pointerPosition(char * currentPointer, char * buffer);
	pipeNode * getPipe(char name[NAME_SIZE]);
	int equalName(char * str1, char * str2);


	void createName(int fromPid, int toPid, char name[NAME_SIZE]);
	int createPidName(int pid, char name[NAME_SIZE], int startPos);
	int inversedId(int id, char aux[NAME_SIZE]);
	int getReceiverPid(char name[NAME_SIZE]);

#endif