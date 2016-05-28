#include "flow.h"

typedef struct heap
{
	long size; // Size of the allocated memory (in number of items)
	long count; // Count of the elements in the heap
	long weight; // total weight of active flows
	Flow **data; // Array with the elements
} FHeap;

int Cmp(Flow *a, Flow *b);
void heap_init(FHeap * h);
void heap_push(FHeap * h, Flow *value);
void heap_pop(FHeap * h);

// Returns the smallest element in the heap
#define heap_front(h) (*(h)->data)

// Frees the allocated memory
#define heap_term(h) (free((h)->data))

//void heapify(Flow *data[restrict], unsigned int count);