#include "FlowsBuffer.h"
// the list of flows
static FHeap * flows;
static int packetCounter = 0;


void InitFlowBuffer()
{
	flows=heap_init();
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

/*
returns the relevent flow for this packet
according to sadd,sport, dadd,dport
from the flows heap - if exist, if not return null
*/
Flow* findFlow(const Packet* p)
{
	Flow* flow;
	for (int i = 0; i < flows->count; i++)
	{
		flow = flows->data[i];
		if (flowComapre(p->net_data, flow->net_data))
			return flow;

	}

	return NULL;
}

Flow* createFlow(const Packet* p)
{
	Flow* f = flow_create(p);
	heap_push(flows, f);
	return f;
}

/*
return relevent flow for the new packet
if not exist , create a new one and push it to the heap
*/
Flow* getFlow(const Packet* p , bool* insertP)
{
	Flow* f = findFlow(p);
	*(insertP) = FALSE;
	if (f == NULL)
	{
		f = createFlow(p);
		*(insertP) = TRUE;
	}
	return f;
}

/*
add packet to the relevent flow
*/
bool buffer_write(Packet* p)
{
	bool insertP = FALSE;
	Flow* f = getFlow(p, &insertP);
	packetCounter++;
	if (insertP == FALSE)//if we didn't insert the packet
	{
		flow_enqueue(f, p);
		if (flows->weight == 0)//in case this flow become active again!
			update_heap_weight(flows, f->weight);

		heapify(flows->data, flows->count);
	}
	else
	{
		//the flow was created now, so we need to update his finish time
		f->prev_finish_time = p->finish_time;
	}
}

bool buffer_isEmpty()
{
	//return packetCounter == 0;
	if (flows->count > 0)
		return (flow_next(heap_front(flows)) == NULL);
	else
		return TRUE;
}

// pop next flow, dequeue next packet, push flow back to heap
Packet* removePacketFromBuffer()
{
	Flow* flow = heap_front(flows);
	heap_pop(flows);
	Packet* pkt = flow_dequeue(flow);
	heap_push(flows, flow);
	return pkt;

}



//get the next flow and the next packet without deleting them from heap
//just return the data
Packet* showNextPacketToTransmit()
{
	Packet* p = NULL;
	Flow* flow;
	if (flows->count == 0)
		return NULL;
	
	flow = heap_front(flows);
	if (flow == NULL)
		return NULL;
	p = (Packet*)queue_front(flow->packets);
	return p;
}


long buffer_getTotalWeight()
{
	return flows->weight;
}

void freeFlows()
{
	for (int i = 0; i < flows->count; i++) flow_free(flows->data[i]);
}



