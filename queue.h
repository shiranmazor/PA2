/*
Queue module
contains methods to manage a FIFO queue of items.
The queue does not take memory ownership for the items it holds.
*/
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdlib.h>
#include <stdio.h>
#include "types.h"

typedef struct t_node
{
	struct t_node* next;
	void* data;
} QueueNode;

typedef struct Queue
{
	QueueNode* front;	// points to first out (oldest)
	QueueNode* newest;	// points to first in (newest)
	int count;
} Queue;

// returns a pointer to a newly allocated empty Queue. caller gets memory responsibility for the Queue.
// returns NULL when fails.
Queue* create_queue();

bool enqueue(Queue* queue, void* data);

// removes an element from the queue. does not free data
bool dequeue(Queue* queue);

void* queue_front(const Queue* queue);

bool queue_isEmpty(const Queue* queue);

#endif