#ifndef _WFQ_H_
#define _WFQ_H_
#include "types.h"
#include "flow.h"

#define INPUT_SIZE 100

bool HandleInputPacket(char* inputLine);
void OutputNextPacket();
void InitScheduler();
bool parseLine(Packet* p, const char* line);


#endif
