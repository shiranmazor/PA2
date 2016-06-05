#include "flow.h"

typedef struct heap
{
	long size; // Size of the allocated memory (in number of items)
	long count; // Count of the elements in the heap
	long weight; // total weight of active flows
	Flow **data; // Array with the elements
	long transmitting_weight;// handle the use case of packet transmitting in gps and a new packet arrives, here weight still counts
} FHeap;

int Cmp(Flow *a, Flow *b);
FHeap* heap_init();
void heap_push(FHeap * h, Flow *value);
void heap_pop(FHeap * h, Flow *value);

// Returns the smallest element in the heap
#define heap_front(h) (*(h)->data)

// Frees the allocated memory
#define heap_term(h) (free((h)->data))

void heapify(Flow ** data, unsigned int count);