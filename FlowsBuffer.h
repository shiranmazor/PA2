/*
holds a collection of flows.
The scheduler writes packets to the buffer, and reads them one by one
using the scheduling algorithm.
*/	
#ifndef _FLOWSBUFFER_H_
#define _FLOWSBUFFER_H_
#include "types.h"
#include "queue.h"
#include "flow.h"
#include "heap.h"
#include <stdio.h>


// writes a single packet to the buffer
bool buffer_write(Packet* p);

// returns TRUE if the buffer does not contain any packets. FALSE - otherwise.
bool buffer_isEmpty();

// returns the first (highest priority) flow in the buffer
//QueueNode* buffer_first();

// callback to be invoked when packet is removed from the buffer, so the buffer can
// update its internal statistics.
void buffer_onPacketRemoved(Flow* flow);

//return the number of active flows 
//long buffer_getActiveLinks();

// clears the buffer and releases any memory allocated by it.
//void buffer_clear();

//return True if they are in the same flow, else false
int flowComapre(const Net* net1, const Net* net2);

Flow* findFlow(const Packet* p);
Flow* getFlow(const Packet* p, bool* insertP);
Flow* createFlow(const Packet* p);

void InitFlowBuffer();
Packet* removePacketFromBuffer();
long buffer_getTotalWeight();
//algorithm functions
Packet* showNextPacketToTransmit();
Round reCalcRoundTime(Round round, Round last_round);
long calcFutureTotalWeight(Packet* leaving_packet);

#endif

