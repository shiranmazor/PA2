#include <stdio.h>
#include "flow.h"


static long lastPriority = 0;

bool flow_enqueue(Flow* flow, Packet* p)
{
	//create new packet
	
	Packet* newp = (Packet*)malloc(sizeof(Packet));
	memcpy(newp, p, sizeof(Packet));
	//memcpy(newp->net_data, flow->net_data, sizeof(Net));
	newp->net_data = flow->net_data;

	if (flow_isEmpty(flow) && p->weight != -1) 
		flow->weight = newp->weight;
	if (flow_isEmpty(flow)) flow->last = newp->finish_time;
	return enqueue(flow->packets, newp);
}

Packet* flow_dequeue(Flow* flow)
{
	Packet* p = (Packet*)queue_front(flow->packets);
	if (!dequeue(flow->packets))
	{
		printf("error removing packet from flow");
		return NULL;
	}
	Packet* flow_first = (Packet*)queue_front(flow->packets);
	if (flow_first != NULL)
		//if there are packets left in queue we will change the flow weight
		if (flow_first->weight != -1)
			flow->weight = flow_first->weight;
	
	return p;
}


Flow* flow_create(Packet *p)
{
	Flow* flow = (Flow*)malloc(sizeof(Flow));
	if (flow == NULL)
	{
		printf("memory allocation failed");
		return NULL;
	}

	flow->net_data = malloc(sizeof(Net));
	flow->net_data->src_addr = p->net_data->src_addr;
	flow->net_data->src_port = p->net_data->src_port;
	flow->net_data->dst_addr = p->net_data->dst_addr;
	flow->net_data->dst_port = p->net_data->dst_port;
	if (p->weight != -1) flow->weight = p->weight;
	else flow->weight = 1;
	flow->packets = create_queue();
	flow->priority = lastPriority++;
	flow->last = 0.0;
	flow_enqueue(flow, p);

	return flow;
}

void flow_free(Flow* flow)
{
	queue_free(flow->packets);
	free(flow->net_data);
	free(flow);
}

Packet* flow_next(Flow* flow)
{
	Packet* p = (Packet*)queue_front(flow->packets);
	return p;
}

bool flow_isEmpty(Flow* flow)
{
	return queue_isEmpty(flow->packets);
}