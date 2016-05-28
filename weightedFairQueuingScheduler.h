#ifndef _WFQ_H_
#define _WFQ_H_
#include <WinSock2.h>
#include "types.h"
#include "flow.h"
#include "FlowsBuffer.h"

#define INPUT_SIZE 100

void HandleInputPackets()
void parseLine(Packet* p, const char* line);
void transmitPacket(Packet pkt);
int main(void);
void calcRound(Packet* p);
bool checkRoundValid(Packet* p);
void calcFinishTime(Packet* p);


#endif
