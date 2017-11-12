#ifndef __MALLOC__

	#define __MALLOC__

	#define NULL ((void *)0)

	typedef struct memBlockNode {
		void * block;
		uint64_t bytes;
		struct memBlockNode * next;
	} memBlockNode;


	void * malloc(uint64_t bytes);

	void free(void * block);

	memBlockNode * addToBlockList(memBlockNode * current, void * block, uint64_t bytes);

	memBlockNode * removeFromBlockList(memBlockNode * current, void * block);

	uint64_t getBlockSize(void * block);

	uint64_t getBlockSizeR(memBlockNode * current, void * block);
	
#endif