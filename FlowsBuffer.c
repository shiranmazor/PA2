#include "FlowsBuffer.h"
// the heap of flows
static FHeap * flows;
static FHeap * virtual_flows; //contain virtual flows for the gps protocol

void InitFlowBuffer()
{
	flows = heap_init();
	virtual_flows = heap_init();
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
Flow* findFlow(const Packet* p,bool virtual_f)
{
	Flow* flow;
	FHeap* heap = virtual_f ? virtual_flows : flows;
	
	for (int i = 0; i < heap->count; i++)
	{
		flow = heap->data[i];
		if (flowComapre(p->net_data, flow->net_data))
			return flow;
	}
	return NULL;
}

Flow* createFlow(Packet* p, bool virtual_f)
{
	Flow* f = flow_create(p);
	if (virtual_f == TRUE)
		heap_push(virtual_flows, f);
	else
		heap_push(flows, f);
	
	return f;
}

/*
return relevent flow for the new packet
if not exist , create a new one and push it to the heap
*/
Flow* getFlow(const Packet* p , bool* insertP, bool virtual_f)
{
	Flow* f = findFlow(p, virtual_f);
	*(insertP) = FALSE;
	if (f == NULL)
	{
		f = createFlow(p, virtual_f);
		*(insertP) = TRUE;
	}
	return f;
}

/*
add packet to the relevent flow
*/
void buffer_write(Packet* p, bool virtual_f)
{
	bool insertP = FALSE;
	Flow* f = getFlow(p, &insertP, virtual_f);
	FHeap* heap = virtual_f ? virtual_flows : flows;
	if (!insertP) //if we didn't insert the packet
	{
		if (flow_isEmpty(f)){ // revived flow
			flow_enqueue(f, p);
			heapify(heap->data, heap->count);
			heap->weight += f->weight;
		}
		else
		{
			flow_enqueue(f, p);
		}

	}

}

bool buffer_isEmpty(bool virtual_f)
{
	FHeap* heap = virtual_f ? virtual_flows : flows;

	if (heap->count > 0)
		return (flow_next(heap_front(heap)) == NULL);
	else
		return TRUE;
}

// pop next flow, dequeue next packet, push flow back to heap
Packet* removePacketFromBuffer(bool virtual_f)
{
	FHeap* heap = virtual_f ? virtual_flows : flows;

	Flow* flow = heap_front(heap);
	heap_pop(heap, flow);
	Packet* pkt = flow_dequeue(flow);
	heap_push(heap, flow);
	return pkt;
}

//get the next flow and the next packet without deleting them from heap
//return the next packet to transmit from virtual heap
Packet* showNextPacketToTransmit(bool virtual_f)
{
	Packet* p = NULL;
	Flow* flow;
	FHeap* heap = virtual_f ? virtual_flows : flows;

	if (heap->count == 0)
		return NULL;

	flow = heap_front(heap);
	if (flow == NULL)
		return NULL;
	p = (Packet*)queue_front(flow->packets);
	return p;
}

long buffer_getTotalWeight()
{
	return virtual_flows->weight;
}

void freeFlows()
{
	for (int i = 0; i < flows->count; i++) 
		flow_free(flows->data[i]);
	free(flows);

	for (int i = 0; i < virtual_flows->count; i++)
		flow_free(virtual_flows->data[i]);
	free(virtual_flows);
}



