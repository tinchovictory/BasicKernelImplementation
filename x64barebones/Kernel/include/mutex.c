#include <mutex.h>


static int counter = 1;

static mutexList list = NULL;


int create(){
	mutexNode * mutex = allocate(sizeof(mutexNode));

	mutex->id = counter++;
	mutex->status = MUTEX_UNBLOCKED;
	mutex->next = NULL;

	add(mutex);	
	
	return mutex->id;
}

void add(mutexNode mutex){
	if(list == NULL){
		* list = allocate(sizeof(mutexList));
		mutex.prev = NULL;
		list.first = mutex;
		list.last = mutex;
		return;
	}
	mutex.prev = list.last;
	list.last.next = mutex;
	list.last = mutex;
	list.length++;
}


void up(int id){
	mutexNode mutex = findID(id);
	if(mutex != NULL){
		mutex.status = MUTEX_UNBLOCKED;
	}
}

void down(int id){
	mutexNode mutex = findID(id);
	if(mutex != NULL){
		mutex.status = MUTEX_BLOCKED;
	}
}

mutexNode * findID(int id){
	mutexNode aux = list.first;
	while(aux != NULL && aux.id != id){
		aux = aux.next;
	}
	return aux;
}

void destroy( int id){
	mutexNode mutex = findID(id);
	if(mutex == NULL){
		return;
	}
	if(list.first.id == id && list.last.id == id){
		list.first = NULL;
		list.last = NULL;
		return;
	}
	if(list.first.id == id){
		list.first = mutex.next;
		list.first.prev = NULL;
		return;
	}
	if(list.last.id == id){
		list.last = mutex.prev;
		list.last.next = NULL;
		return;
	}

}

int isBlocked(id){
	mutexNode mutex = finID(id);
	if(mutex == NULL){
		return NULL;
	}
	return !mutex.status;
}