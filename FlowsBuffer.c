#include "FlowsBuffer.h"
// the list of flows
static FHeap flows;
static int packetCounter = 0;


void InitFlowBuffer()
{
	heap_init(&flows);
}

int flowComapre(const Net* net1, const Net* net2)
{
	if ((net1->dst_addr.S_un.S_addr == net2->dst_addr.S_un.S_addr)
		&& (net1->dst_port == net2->dst_port)
		&& (net1->src_addr.S_un.S_addr == net2->src_addr.S_un.S_addr)
		&& (net1->src_port == net2->src_port))
		return TRUE;
	else
		return FALSE;
}

Flow* findFlow(const Packet* p)
{
	Flow* flow;
	for (int i = 0; i < flows.count; i++)
	//while (it != NULL)
	{
		flow = flows.data[i];
		//Flow* flow = (Flow*)it->data;
		if (flowComapre(&(p->net_data), &(flow->net_data)) == 0)
			return flow;

		//it = it->next;
	}

	return NULL;
}

Flow* createFlow(const Packet* p)
{
	Flow* f = flow_create(p, buffer_onPacketRemoved);
	heap_push(&flows, f);
	return f;
}

Flow* getFlow(const Packet* p)
{
	Flow* f = findFlow(p);
	if (f == NULL)
		f = createFlow(p);

	return f;
}

void buffer_onPacketRemoved(Flow* flow)
{
	packetCounter--;
}

bool buffer_write(Packet* p)
{
	Flow* f = getFlow(p);
	packetCounter++;
	return enqueue(f->packets, p);
}

bool buffer_isEmpty()
{
	//return packetCounter == 0;
	return (flow_next(flows.data[flows.count - 1]) == NULL);
}


//QueueNode* buffer_first()
//{
//	return flows.front;
//}

// pop next flow, dequeue next packet, push flow back to heap
Packet* getPacketToTransmit()
{
	Flow* flow = heap_front(&flows);
	heap_pop(&flows);
	Packet* pkt = flow_dequeue(flow);
	heap_push(&flows, flow);
	return pkt;
}

long buffer_getTotalWeight()
{
	return flows.weight;
}

/*
count the flows with packets in his packets queue

long buffer_getActiveLinks()
{
	int activeLinks = 0;
	QueueNode* current = flows.front;
	while (current != NULL)
	{
		Flow* flow = (Flow*)current->data;
		if (queue_isEmpty(flow->packets) == FALSE)
			activeLinks++;

		current = current->next;
	}

	return activeLinks;

}
*/