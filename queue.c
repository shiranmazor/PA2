#include "queue.h"


Queue* create_queue()
{
	Queue* q = (Queue*)malloc(sizeof(Queue));
	if (q == NULL)
	{
		printf("mem allocation failed");
		return NULL;
	}

	q->front = NULL;
	q->newest = NULL;
	q->count = 0;

	return q;
}

bool enqueue(Queue* queue, void* data)
{
	QueueNode* temp = (QueueNode*)malloc(sizeof(QueueNode));
	if (temp == NULL)
	{
		printf("memory allocation failed");
		return FALSE;
	}

	temp->data = data;
	temp->next = NULL;

	if (queue->front == NULL && queue->newest == NULL)
	{
		queue->front = queue->newest = temp;
		queue->count++;
		return TRUE;
	}

	queue->newest->next = temp;
	queue->newest = temp;
	queue->count++;

	return TRUE;
}

bool dequeue(Queue* queue)
{
	QueueNode* temp = queue->front;
	if (queue->front == NULL)
	{
		printf("Queue is empty\n");
		return FALSE;
	}

	if (queue->front == queue->newest)
		queue->front = queue->newest = NULL;
	else
		queue->front = queue->front->next;

	queue->count--;

	free(temp);

	return TRUE;
}

void* queue_front(const Queue* queue)
{
	if (queue->front == NULL)
		return NULL;

	return queue->front->data;
}
//get second node after the front
void* queue_second(const Queue* queue)
{
	if (queue->front == NULL)
		return NULL;
		
	return queue->front->next->data;

}

bool queue_isEmpty(const Queue* queue)
{
	return (queue->count == 0);
}