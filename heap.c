#include <stdlib.h>

#include "heap.h"
#include "flow.h"
#include "types.h"

static const int base_size = 100;

int Cmp(Flow *a, Flow *b)
{
	Packet* a_pkt = flow_next(a);
	Packet* b_pkt = flow_next(b);
	if (a_pkt == NULL) return 0; // a is empty, put at heap bottom
	if (b_pkt == NULL) return 1; // b is empty, put at heap bottom
	if (a_pkt->finish_time == b_pkt->finish_time) return a->priority < b->priority;
	return a_pkt->finish_time <= b_pkt->finish_time;
}

/*
Prepares the heap for use
*/
FHeap* heap_init()
{
	FHeap* h = malloc(sizeof(FHeap));
	h->size = base_size;
	h->count = 0;
	h->weight = 0;
	h->transmitting_weight = 0;
	h->data = malloc(sizeof(Flow *) * base_size);
	if (!h->data) _exit(1); // Exit if the memory allocation fails
	return h;
}


/*
Inserts element to the heap
*/
void heap_push(FHeap * h, Flow *value)
{
	unsigned int index, parent;

	// Resize the heap if it is too small to hold all the data
	if (h->count == h->size)
	{
		h->size <<= 1;
		h->data = realloc(h->data, sizeof(Flow *) * h->size);
		if (!h->data) _exit(1); // Exit if the memory allocation fails
	}

	// Find out where to put the element and put it
	for (index = h->count++; index; index = parent)
	{
		parent = (index - 1) >> 1;
		if (Cmp(h->data[parent], value)) 
			break;
		h->data[index] = h->data[parent];
	}
	h->data[index] = value;

	if (!flow_isEmpty(value)) h->weight += value->weight;
}

/*
remove the smallest element from the heap
*/
void heap_pop(FHeap * h, Flow *value)
{
	int index, swap, other;

	// update total weight
	h->weight -= heap_front(h)->weight;

	// Remove the last element
	Flow* temp = h->data[--h->count];

	// Resize the heap if it's consuming too much memory
	if ((h->count <= (h->size >> 2)) && (h->size > base_size))
	{
		h->size >>= 1;
		h->data = realloc(h->data, sizeof(Flow *) * h->size);
		if (!h->data) _exit(1); // Exit if the memory allocation fails
	}

	// Reorder the elements
	for (index = 0; 1; index = swap)
	{
		// Find the child to swap with
		swap = (index << 1) + 1;
		if (swap >= h->count) break; // If there are no children, the heap is reordered
		other = swap + 1;
		if ((other < h->count) && Cmp(h->data[other], h->data[swap])) swap = other;
		if (Cmp(temp, h->data[swap]))
			break; // If the bigger child is less than or equal to its parent, the heap is reordered

		h->data[index] = h->data[swap];
	}
	h->data[index] = temp;


	if (h->weight < 0)
		printf("heap weight error!! curr weight %d\n", h->weight);
}


// Heapifies a non-empty array
void heapify(Flow** data, unsigned int count)
{
	unsigned int item, index, swap, other;
	Flow * temp;

	// Move every non-leaf element to the right position in its subtree
	if (count < 2)
		return;
	item = (count >> 1) - 1;
	while (1)
	{
		// Find the position of the current element in its subtree
		temp = data[item];
		for (index = item; 1; index = swap)
		{
			// Find the child to swap with
			swap = (index << 1) + 1;
			if (swap >= count) break; // If there are no children, the current element is positioned
			other = swap + 1;
			if ((other < count) && Cmp(data[other], data[swap])) swap = other;
			if (Cmp(temp, data[swap])) break; // If the bigger child is smaller than or equal to the parent, the heap is reordered

			data[index] = data[swap];
		}
		if (index != item) data[index] = temp;

		if (!item) return;
		--item;
	}
}