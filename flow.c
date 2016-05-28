#include <stdio.h>
#include "flow.h"


static long lastPriority = 0;

bool flow_enqueue(Flow* flow, Packet* p)
{
	return enqueue(flow->packets, p);
}

Packet* flow_dequeue(Flow* flow)
{
	Packet* p = (Packet*)queue_front(flow->packets);
	flow->OnPacketRemoved(flow);
	if (!dequeue(flow->packets))
	{
		printf("error removing packet from flow");
		return NULL;
	}

	return p;
}


Flow* flow_create(Packet *p, void(*callback)(struct TFlow* flow))
{
	Flow* flow = (Flow*)malloc(sizeof(Flow));
	if (flow == NULL)
	{
		printf("memory allocation failed");
		return NULL;
	}

	flow->net_data = p->net_data;
	flow->weight = p->weight;
	flow->packets = create_queue();
	flow->priority = lastPriority++;
	flow->OnPacketRemoved = callback;
	flow_enqueue(flow, p);

	return flow;
}

void flow_free(Flow* flow)
{
	free(flow->packets);
	free(flow);
}

Packet* flow_next(Flow* flow)
{
	Packet* p = (Packet*)queue_front(flow->packets);
	return p;
}