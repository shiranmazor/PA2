#ifndef _WFQ_H_
#define _WFQ_H_
#include "types.h"
#include "flow.h"

#define INPUT_SIZE 100

bool HandleInputPacket();
bool parseLine(Packet* p, const char* line);
void transmitPacket(Packet pkt);
int main(void);

static timer time;
static unsigned long long transmitting;
static Packet* next_packet;

#endif
