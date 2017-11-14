#include <stdio.h>
//#include <string.h>
#include <systemCalls.h>
#include "philosophers.h"
/*
void myTestSleep(int wakeUpCall);

int auxCounter = 0;
int mainMutexId;
int philoMutexId[PHILOSOPHERS_COUNT];
int philosopherId[PHILOSOPHERS_COUNT];
char * stateStrings[3] = { "Hungry", "Thinking", "Eating" };
State philoState[PHILOSOPHERS_COUNT];
int forkState[PHILOSOPHERS_COUNT];

void philosophers() {
	int pid = getCurrentPid();
	int i; 
	mainMutexId = createMutex();
	for(i = 0; i < PHILOSOPHERS_COUNT; i++){
		//initialize forks
		philoMutexId[i] = createMutex();
		mutexDown(philoMutexId[i]);
		forkState[i] = -1;
		//initialize philosophers
		philosopherId[i] = i;
		philoState[i] = THINKING;
		philosopherId[i] = tcreate(pid, philosopher);
	}
	printf("running\n");


	//exit();
	while(1);
}

void philosopher() {
	int id = auxCounter;
	auxCounter++;
	while(1) {
		//Think
		myTestSleep(10000);
		takeForks(id);

		//Eat
		myTestSleep(10000);
		putForks(id);
	}
}

void checkForks(int philoId) {
	if(philoState[philoId] == HUNGRY && 
		philoState[left(philoId)] != EATING && 
		philoState[right(philoId)] != EATING){
		//forks available
		philoState[philoId] = EATING;
		forkState[left(philoId)] = philoId;
		forkState[philoId] = philoId;
		render();

		mutexUp(philoMutexId[philoId]); //Pick up forks
	}
}

void takeForks(int philoId) {
	mutexDown(mainMutexId);

	//Set state
	philoState[philoId] = HUNGRY;
	render();

	checkForks(philoId);
	mutexUp(mainMutexId);
	mutexDown(philoMutexId[philoId]); //Lock if forks not acquired
}

void putForks(int philoId) {
	mutexDown(mainMutexId);

	philoState[philoId] = THINKING;
	forkState[left(philoId)] = -1;
	forkState[philoId] = -1;
	render();

	checkForks(left(philoId)); //philosopher on my left tries to acquire forks
	checkForks(right(philoId)); //philosopher on my right tries to acquire forks
	mutexUp(mainMutexId);

}

int left(int i) {
	return (i + PHILOSOPHERS_COUNT - 1) % PHILOSOPHERS_COUNT;
}

int right(int i) {
	return (i + 1) % PHILOSOPHERS_COUNT;
}

void render() {
	int i;
	for(i = 0; i < PHILOSOPHERS_COUNT; i++) {
		printf("Philosopher %d: %s\n", i, stateStrings[philoState[i]]);
		printf("Fork - ");

		if (forkState[i] == -1)
			printf("Free\n");
		else
			printf("Owner %d\n", forkState[i]);
	}

	putchar('\n');
	putchar('\n');
}

void myTestSleep(int wakeUpCall) {
	int i = 0;
	while(i < wakeUpCall){
		i++;
	}
}*/