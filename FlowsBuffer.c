#include "FlowsBuffer.h"
// the list of flows
static FHeap * flows;
static FHeap * virtual_flows;//contain virtual flows for the gps protocol



void InitFlowBuffer()
{
	flows=heap_init();
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
	if (virtual_f == TRUE)
	{
		for (int i = 0; i < virtual_flows->count; i++)
		{
			flow = virtual_flows->data[i];
			if (flowComapre(p->net_data, flow->net_data))
				return flow;

		}
	}
	else
	{
		for (int i = 0; i < flows->count; i++)
		{
			flow = flows->data[i];
			if (flowComapre(p->net_data, flow->net_data))
				return flow;

		}
	}
	

	return NULL;
}

Flow* createFlow(const Packet* p, bool virtual_f)
{
	Flow* f = flow_create(p, virtual_f);
	if (virtual_f == TRUE)
	{
		heap_push(virtual_flows, f);

	}
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
bool buffer_write(Packet* p, bool virtual_f)
{
	bool insertP = FALSE;
	Flow* f = getFlow(p, &insertP, virtual_f);
	if (!insertP) //if we didn't insert the packet
	{
		if (flow_isEmpty(f)){ // revived flow
			flow_enqueue(f, p);
			if (virtual_f)
			{
				heapify(virtual_flows->data, virtual_flows->count);
				virtual_flows->weight += f->weight;
			}
				
			else
			{
				heapify(flows->data, flows->count);
				flows->weight += f->weight;
			}
							
		}
		else
		{
			flow_enqueue(f, p);
		}

	}

}

bool buffer_isEmpty(bool virtual_f)
{
	if (virtual_f == TRUE)
	{
		if (virtual_flows->count > 0)
			return (flow_next(heap_front(virtual_flows)) == NULL);
		else
			return TRUE;
	}
	else
	{
		if (flows->count > 0)
			return (flow_next(heap_front(flows)) == NULL);
		else
			return TRUE;
	}
	
}

Packet* getPacketFromBuffer()
{
	Packet* pkt = NULL;
	Flow* flow = NULL;
	if (virtual_flows->count == 0)
		return NULL;

	flow = heap_front(virtual_flows);
	if (flow == NULL)
		return NULL;
	pkt = (Packet*)queue_front(flow->packets);
	return pkt;
}

// pop next flow, dequeue next packet, push flow back to heap
Packet* removePacketFromBuffer(bool virtual_f)
{
	if (virtual_f)
	{
		Flow* flow = heap_front(virtual_flows);
		heap_pop(virtual_flows, flow);
		Packet* pkt = flow_dequeue(flow);
		heap_push(virtual_flows, flow);
		return pkt;
	}
	else
	{
		Flow* flow = heap_front(flows);
		heap_pop(flows, flow);
		Packet* pkt = flow_dequeue(flow);
		//printf("round_v %f round %ld f_time %f", pkt->arrival_time.round_val, pkt->arrival_time.round_time, pkt->finish_time);
		
		heap_push(flows, flow);
		return pkt;
	}
	

}



//get the next flow and the next packet without deleting them from heap
//return the next packet to transmit from virtual heap
Packet* showNextPacketToTransmit(bool virtual_f)
{
	Packet* p = NULL;
	Flow* flow;
	if (virtual_f)
	{
		if (virtual_flows->count == 0)
			return NULL;

		flow = heap_front(virtual_flows);
		if (flow == NULL)
			return NULL;
		p = (Packet*)queue_front(flow->packets);
		return p;

	}
	else
	{
		if (flows->count == 0)
			return NULL;

		flow = heap_front(flows);
		if (flow == NULL)
			return NULL;
		p = (Packet*)queue_front(flow->packets);
		return p;
	}
	
}
long buffer_getTotalWeight(bool virtual_f)
{
	if (virtual_f)
		return virtual_flows->weight;
	else
		return flows->weight;
}
void update_trasmittingWeight(long weight)
{
	flows->transmitting_weight = weight;
}

void getActiveLinks()
{
	int active_links = 0;
	long total_count = 0;
	for (int i = 0; i < flows->count; i++)
	{
		Flow* curr = flows->data[i];
		if (flow_isEmpty(curr) == FALSE)
		{
			total_count = total_count + curr->weight;
			active_links++;
		}

	}
	printf("Active links: %d total weight: %ld\n", active_links, total_count);
}

long get_trasmittingWeight()
{
	return flows->transmitting_weight;
}
/*
void pendingPacketWeight(Packet* p, int sign)
{
Flow* f = findFlow(p);
if (!f)
extra_weight += sign * (p->weight == -1 ? 1 : p->weight);
else if (flow_isEmpty(f))
extra_weight += sign * (p->weight == -1 ? f->weight : p->weight);
}
*/


void freeFlows()
{
	for (int i = 0; i < flows->count; i++) 
		flow_free(flows->data[i]);
	free(flows);

	for (int i = 0; i < virtual_flows->count; i++)
		flow_free(virtual_flows->data[i]);
	free(virtual_flows);
}



