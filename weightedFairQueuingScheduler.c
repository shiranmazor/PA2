#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include "weightedFairQueuingScheduler.h"

static timer time;
static unsigned long long transmitting;
Packet* next_packet;

/*
line format:
time Sadd Sport Dadd Dport length [weight]

*/
bool parseLine(Packet* p, const char* line)
{
	int parameterIndex = 0;
	char * pch;
	pch = strtok(line, " ");
	while (pch != NULL)
	{
		if (parameterIndex == 0)
			p->time = atol(pch);
		if (parameterIndex == 1)
			p->net_data.src_addr.S_un.S_addr = inet_addr(pch);
		if (parameterIndex == 2)
			p->net_data.src_addr.S_un.S_addr = inet_addr(pch);
		if (parameterIndex == 3)
			p->net_data.src_addr.S_un.S_addr = inet_addr(pch);
		if (parameterIndex == 4)
			p->net_data.src_addr.S_un.S_addr = inet_addr(pch);
		if (parameterIndex == 5)
			p->net_data.src_addr.S_un.S_addr = inet_addr(pch);
		if (parameterIndex == 6)
			p->net_data.src_addr.S_un.S_addr = inet_addr(pch);

		pch = strtok(NULL, " ");
		parameterIndex++;
	}
}

// calc next_packet virtual finish time and insert to relevant queue 
bool HandleInputPacket()
{
	
}


int main(void)
{
	char line[INPUT_SIZE];
	time = 0;
	transmitting = 0;
	next_packet = (Packet*)malloc(sizeof(Packet));
	if (next_packet == NULL)
	{
		printf("packet allocation failed");
		return NULL;
	}

	if (fgets(line, INPUT_SIZE, stdin) != NULL)
		parseLine(next_packet, line);

	do{
		// handle input at this time
		if (next_packet->time == time){
			HandleInputPacket();
			if (fgets(line, INPUT_SIZE, stdin) != NULL)
				parseLine(next_packet, line);
			continue;
		}

		// handle output
		if (transmitting == 0){
			// begin transmition of next packet
			// transmitting = packet_to_be_transmitted->time;
		}

		// advance time
		time++;
		transmitting--;
	} while (1); // CHANGE TO while packet queue not empty
	return 0;
}