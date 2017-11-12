#include <stdint.h>
#include <memoryManager.h>
#include "malloc.h"

static memBlockNode * assignedMemBlocksList = NULL;

void * malloc(uint64_t bytes) {
	void * physicalMem = allocate(bytes);

	assignedMemBlocksList = addToBlockList(assignedMemBlocksList, physicalMem, bytes);

	return physicalMem;
}

void free(void * block) {
	uint64_t blockSize = getBlockSize(block);
	deallocate(block, blockSize);

	assignedMemBlocksList = removeFromBlockList(assignedMemBlocksList, block);
}

memBlockNode * addToBlockList(memBlockNode * current, void * block, uint64_t bytes) {
	if(current == NULL) {
		memBlockNode * node = (memBlockNode *) allocate(sizeof(memBlockNode));
		node->block = block;
		node->bytes = bytes;
		return node;
	}
	current->next = addToBlockList(current->next, block, bytes);
	return current;
}

memBlockNode * removeFromBlockList(memBlockNode * current, void * block) {
	if(current == NULL) {
		return NULL;
	}
	if(current->block == block) {
		memBlockNode * aux = current->next;
		deallocate(current, sizeof(memBlockNode));
		return aux;
	}
	current->next = removeFromBlockList(current->next, block);
	return current;
}

uint64_t getBlockSize(void * block) {
	return getBlockSizeR(assignedMemBlocksList, block);
}

uint64_t getBlockSizeR(memBlockNode * current, void * block) {
	if(current == NULL) {
		return 0;
	}
	if(current->block == block) {
		return current->bytes;
	}
	return getBlockSizeR(current->next, block);
}