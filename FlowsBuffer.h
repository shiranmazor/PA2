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

// used for taking into account the weight of packets waiting in the incoming packets queue
static long transmitting_weight;
static long extra_weight;

// writes a single packet to the buffer
bool buffer_write(Packet* p);

// returns TRUE if the buffer does not contain any packets. FALSE - otherwise.
bool buffer_isEmpty();


//return True if they are in the same flow, else false
int flowComapre(const Net* net1, const Net* net2);

Flow* findFlow(const Packet* p);
Flow* getFlow(const Packet* p, bool* insertP);
Flow* createFlow(const Packet* p);
void freeFlows();

void InitFlowBuffer();
Packet* removePacketFromBuffer();
Packet* showNextPacketToTransmit();
long buffer_getTotalWeight();

// used for taking into account the weight of packets waiting in the incoming packets queue
void pendingPacketWeight(Packet* p, int sign);

#endif

