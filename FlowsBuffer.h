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
bool buffer_write(Packet* p, bool virtual_f);

// returns TRUE if the buffer does not contain any packets. FALSE - otherwise.
bool buffer_isEmpty(bool virtual_f);


//return True if they are in the same flow, else false
int flowComapre(const Net* net1, const Net* net2);

Flow* findFlow(const Packet* p, bool virtual_f);
Flow* getFlow(const Packet* p, bool* insertP, bool virtual_f);
Flow* createFlow(const Packet* p, bool virtual_f);
void freeFlows();

void InitFlowBuffer();
Packet* removePacketFromBuffer(bool virtual_f);
Packet* showNextPacketToTransmit(bool virtual_f);

long buffer_getTotalWeight(bool virtual_f);
void update_trasmittingWeight(long weight);
long get_trasmittingWeight();
Packet* getPacketFromBuffer();
void getActiveLinks();
// used for taking into account the weight of packets waiting in the incoming packets queue
//void pendingPacketWeight(Packet* p, int sign);

#endif

