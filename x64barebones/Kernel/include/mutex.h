#define MUTEX_BLOCKED 1
#define MUTEX_UNBLOCKED 2

typedef struct{
	mutexNode * prev;
	int id;
	int status;
	mutexNode * next;
} mutexNode;

typedef struct{
	mutexNode * first;
	mutexNode * last;
	int length;
} mutexList;

int create(int value);

void up(int id);

void down(int id);

void destroy( int id);

int isBlocked(id);