/*
Flow module
contains methods and types which are used to work with Flows.
A flow is a FIFO queue of packets of the same Endpoint (srcport,srcaddr,dstport,dstaddr).
*/
#ifndef _FLOW_H_
#define _FLOW_H_
#include "types.h"
#include "queue.h"

typedef struct TFlow
{
	long weight;	// weight. determined by first packet in the flow
	Net* net_data;	// this identifies the flow
	long priority; // determine by time arrival time
	Queue* packets;		// the FIFO queue of packets
	double last;
} Flow;

// removes a packet from the flow. the packet is returned by the method.
Packet* flow_dequeue(Flow* flow);

// inserts a packet to the flow. returns TRUE if succeeded, FALSE if not.
bool flow_enqueue(Flow* flow, Packet* p);

// creates a new flow.
Flow* flow_create(Packet *p);

// frees a flow
void flow_free(Flow* flow);

// returns a pointer to the next packet in the flow
Packet* flow_next(Flow* flow);

bool flow_isEmpty(Flow* flow);

#endif