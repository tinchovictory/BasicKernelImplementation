#include <mutex.h>
#include <naiveConsole.h>

static int mutex[5];

int main(){
	int mutex[5];
	int i;
	for(i=0; i<5; i++){
		mutex[i] = createTest();
	}

}


int createTest();

void upTest(int id);

void downTest(int id);

void destroyTest( int id);

int isBlockedTest(id);


int createTest(){
	int mutex = create();
	ncPrintDec(mutex);
	return mutex;
}

void upTest(int id){

	assert(mutex[2] == MUTEX_UNBLOCKED);

	up(mutex[2]);

	assert(mutex[2] == MUTEX_UNBLOCKED);

}

void downTest(int id){

	assert(mutex[4] == MUTEX_UNBLOCKED);

	down(mutex[4]);

	assert(mutex[4] == MUTEX_UNBLOCKED);
}

void destroyTest( int id){
	
}

int isBlockedTest(id);

