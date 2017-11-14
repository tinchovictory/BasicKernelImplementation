#ifndef PHILOSOPHERS_H
#define PHILOSOPHERS_H

#define PHILOSOPHERS_COUNT 3

typedef enum {HUNGRY, THINKING, EATING} State;

void philosophers();

void philosopher();

void checkForks(int philoId);

void takeForks(int philoId);

void putForks(int philoId);

int left(int i);

int right(int i);

void render();

void myTestSleep(int wakeUpCall);

#endif
