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
		if (flowComapre(&(p->net_data), &(flow->net_data)) == 0)
			return flow;

	}

	return NULL;
}

Flow* createFlow(const Packet* p)
{
	Flow* f = flow_create(p, buffer_onPacketRemoved);
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

void buffer_onPacketRemoved(Flow* flow)
{
	packetCounter--;
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
		return flow_enqueue(f->packets, p);
}

bool buffer_isEmpty()
{
	//return packetCounter == 0;
	return (flow_next(flows->data[flows->count - 1]) == NULL);
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
	Flow* flow = heap_front(flows);
	Packet* p = (Packet*)queue_front(flow->packets);
	return p;
}

/*
check if current roundtime need to be changed:
if round time is bigger then finish time of some packets:
for each packet we found - check if the nexr packet in queue change the flow weight , if so recalc the round time
according to the packets finish time
*/
Round reCalcRoundTime(Round round, Round last_round)
{
	Round newRound = round;
	//get the next leaving packet - min packet from heap
	Packet* next_p = showNextPacketToTransmit();
	if (next_p->finish_time < round.round_val)
	{
		//get future total weights
		long oldTotalWeight = buffer_getTotalWeight();
		long newTotalWeights = calcFutureTotalWeight(next_p);
		//find the new x value = the actual time  next p finishtime:
		double x = (next_p->finish_time * oldTotalWeight) - (oldTotalWeight *last_round.round_val);
		//update last round to be the finish time
		last_round.round_time = x;
		last_round.round_val = next_p->finish_time;
		newRound.round_val = last_round.round_val + (double)(x / newTotalWeights);

	}
	return newRound;
}

long buffer_getTotalWeight()
{
	return flows->weight;
}
/*
calc future flows weight - without changing it actually
*/
long calcFutureTotalWeight(Packet* leaving_packet)
{
	Flow* packets_flow = findFlow(leaving_packet);
	long oldFlowWeight = packets_flow->weight;
	//get next packet in packet_queue
	Packet* next_packet = queue_second(packets_flow->packets);
	if (next_packet == NULL )
	{
		///weight remain the same
		return buffer_getTotalWeight();
	}
	else
	{
		if (next_packet->weight == 0)
			return buffer_getTotalWeight();//packet arrived without weight so the flow weight isn't changing
		else
		{
			
			long newFlowWeight = next_packet->weight;
			//substruct the old flow weight and update the total weight with the new one
			long totalFlowsWeight = buffer_getTotalWeight() - oldFlowWeight + newFlowWeight;
			return totalFlowsWeight;
		}
	}
		
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

