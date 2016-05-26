#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include "weightedFairQueuingScheduler.h"
#include "FlowsBuffer.h"

static unsigned long long time;
static unsigned long long transmitting;
static Packet* next_packet;

/*
line format:
time Sadd Sport Dadd Dport length [weight]
*/
void parseLine(Packet* p, const char* line)
{
	int parameterIndex = 0;
	char * pch;
	p->weight = 1; // weight default
	pch = strtok(line, " ");
	while (pch != NULL)
	{
		if (parameterIndex == 0)
			p->time = atol(pch);
		if (parameterIndex == 1)
			p->net_data.src_addr.S_un.S_addr = inet_addr(pch);
		if (parameterIndex == 2)
			p->net_data.src_port = atoi(pch);
		if (parameterIndex == 3)
			p->net_data.dst_addr.S_un.S_addr = inet_addr(pch);
		if (parameterIndex == 4)
			p->net_data.dst_port = atoi(pch);
		if (parameterIndex == 5)
			p->length = atoi(pch);
		if (parameterIndex == 6)
			p->weight = atoi(pch);

		pch = strtok(NULL, " ");
		parameterIndex++;
	}
}

// calc next_packet virtual finish time and insert to relevant queue 
bool HandleInputPacket()
{
	calcRound(next_packet); // ?
	calcFinishTime(next_packet);

	buffer_write(next_packet);
	return 0;
}

void transmitPacket(Packet pkt)
{
	printf("%d: %s %d %s %d %d", time, inet_ntoa(pkt.net_data.src_addr), pkt.net_data.src_port, inet_ntoa(pkt.net_data.dst_addr), pkt.net_data.dst_port, pkt.length);
	transmitting = pkt.length;
}

int main(void)
{
	char line[INPUT_SIZE];
	InitFlowBuffer();
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
			Packet* pkt = getPacketToTransmit();
			transmitPacket(*pkt);
			free(pkt);
		}

		// advance time
		time++;
		transmitting--;
	} while (!buffer_isEmpty());
	
	free(next_packet);
	return 0;
}