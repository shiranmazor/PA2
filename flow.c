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

Flow* flow_create(const Net* net_data, long weight)
{
	Flow* flow = (Flow*)malloc(sizeof(Flow));
	if (flow == NULL)
	{
		printf("memory allocation failed");
		return NULL;
	}

	flow->net_data = *net_data;
	flow->weight = weight;
	flow->packets = create_queue();
	flow->priority = lastPriority++;

	return flow;
}

void flow_free(Flow* flow)
{
	free(flow->packets);
	free(flow);
}

