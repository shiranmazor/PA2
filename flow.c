#include <stdio.h>
#include "flow.h"


static long lastPriority = 0;

bool flow_enqueue(Flow* flow, Packet* p)
{
	////create new packet in memory
	//Packet* insertP = (Packet*)malloc(sizeof(Packet));
	//insertP->arrival_time = p->arrival_time;
	//insertP->finish_time = p->finish_time;
	//insertP->length = p->length;
	//insertP->net_data = p->net_data;
	//insertP->time = p->time;
	//insertP->weight = p->weight;
	//return enqueue(flow->packets, insertP);
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
	Packet* flow_first = (Packet*)queue_front(flow->packets);
	if (flow_first != NULL)
	{
		//if there are packets left in queue we will change the flow weight
		if (flow_first->weight != -1)
			flow->weight = flow_first->weight;
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
	if (p->weight != -1) flow->weight = p->weight;
	else flow->weight = 1;
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