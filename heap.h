#ifndef _FHEAP_
#define _FHEAP_
#include "flow.h"

typedef struct heap
{
	long size; // Size of the allocated memory (in number of items)
	long count; // Count of the elements in the heap
	long weight; // total weight of active flows
	Flow **data; // Array with the elements
} FHeap;

int Cmp(Flow *a, Flow *b);
FHeap* heap_init();
void heap_push(FHeap * h, Flow *value);
void heap_pop(FHeap * h, Flow *value);
void heapify(Flow ** data, unsigned int count);

// Returns the smallest element in the heap
#define heap_front(h) (*(h)->data)

#endif