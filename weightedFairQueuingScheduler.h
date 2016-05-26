#ifndef _WFQ_H_
#define _WFQ_H_
#include "types.h"
#include "flow.h"

#define INPUT_SIZE 100

bool HandleInputPacket();
void parseLine(Packet* p, const char* line);
void transmitPacket(Packet pkt);
int main(void);

static unsigned long long time;
static unsigned long long transmitting;
static Packet* next_packet;

#endif
