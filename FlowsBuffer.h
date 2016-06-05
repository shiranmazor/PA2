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
void buffer_write(Packet* p, bool virtual_f);

// returns TRUE if the buffer does not contain any packets. FALSE - otherwise.
bool buffer_isEmpty(bool virtual_f);

// returns the total weight of active flows in the virtual buffer (GPS)
long buffer_getTotalWeight();

void InitFlowBuffer();
int flowComapre(const Net* net1, const Net* net2);
Flow* findFlow(const Packet* p, bool virtual_f);
Flow* createFlow(Packet* p, bool virtual_f);
Flow* getFlow(const Packet* p, bool* insertP, bool virtual_f); // get or create flow
void freeFlows();

Packet* removePacketFromBuffer(bool virtual_f);
Packet* showNextPacketToTransmit(bool virtual_f);

#endif

