#pragma warning(disable: 4996)
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "weightedFairQueuingScheduler.h"


static unsigned long long time;
static unsigned long long transmitting;
static Packet* next_packet;
//global variable for the current round of the system according to the incoming packets
//used only for round calculations for each  recive packet
static Round last_round;
/*
line format:
time Sadd Sport Dadd Dport length [weight]
*/
void parseLine(Packet* p, const char* line)
{
	int parameterIndex = 0;
	char * pch;
	p->weight = -1; // weight default
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

/*
calc arrival time for the packet using last round
*/
void calcRound(Packet* p, int x)
{
	long active_links_weights = buffer_getTotalWeight();
	if (active_links_weights == 0)
	{
		//round(0)=0
		p->arrival_time.round_time = 0;
		p->arrival_time.round_val = 0;
	}		 
	else
	{
		p->arrival_time.round_time = last_round.round_time + x;
		p->arrival_time.round_val = last_round.round_val + (double)(x / active_links_weights);

		//check if current packet arrival time is bigger then some packet finish time,
		//in that case - recalculate the flows total weight and change the curret round time
		p->arrival_time = reCalcRoundTime(p->arrival_time, last_round);		
		
	}
	last_round = p->arrival_time;
}

/*
calc last_pi of current packet 
*/
void calcFinishTime(Packet* p)
{	
	double prev_last_pi;//last
	//get relevent flow and extract the last_p(i-1) of the previous packet
	Flow* packet_flow = findFlow(p);
	if (packet_flow == NULL)//this packet still don't have a flow she's the first in here flow
		prev_last_pi = 0;
	else
	{
		//get previous packet
		if (packet_flow->packets->count > 0)
		{
			Packet* last_packet = (Packet*)packet_flow->packets->newest->data;
			prev_last_pi = last_packet->finish_time;
		}
		else
			prev_last_pi = 0;		
	}
	//now we calc the finish time according to the furmula in the reaction:
	long packet_flow_weight = 1;//default weight
	if (packet_flow != NULL)
		packet_flow_weight = packet_flow->weight;

	if (p->weight > 0)//weight has arrived
		packet_flow_weight = p->weight;

	p->finish_time = MAX(last_round.round_val, prev_last_pi) + (double)(p->length / packet_flow_weight);
	
}
// calc next_packet virtual finish time and insert to relevant queue 
void HandleInputPacket(long time_delta)
{
	//for each packet we will calc round (even\arrival time) and last_pi (finish time)
	calcRound(next_packet, time_delta);
	calcFinishTime(next_packet);

	//insert the packet to the flows heap to the relevet flow
	buffer_write(next_packet);
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
	long time_delta = 0;
	transmitting = 0;
	next_packet = (Packet*)malloc(sizeof(Packet));
	if (next_packet == NULL)
	{
		printf("packet allocation failed");
		return NULL;
	}

	if (fgets(line, INPUT_SIZE, stdin) != NULL)
		parseLine(next_packet, line);//fill in packet

	do
	{
		// handle input at this time
		if (next_packet->time == time)
		{
			long x = time_delta;
			time_delta = 0;
			HandleInputPacket(x);
			if (fgets(line, INPUT_SIZE, stdin) != NULL)
				parseLine(next_packet, line);
			continue;
		}

		// handle output
		if (transmitting == 0)
		{
			// begin transmition of next packet
			Packet* pkt = getPacketToTransmit();
			transmitPacket(*pkt);
			free(pkt);
		}

		// advance time
		time++;
		time_delta++;
		transmitting--;
		
	} while (!buffer_isEmpty());
	
	free(next_packet);
	return 0;
}