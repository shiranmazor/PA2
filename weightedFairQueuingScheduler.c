#pragma warning(disable: 4996)
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "weightedFairQueuingScheduler.h"


static unsigned long long time;
static unsigned long long last_time;
static unsigned long long transmitting;
static Packet* next_packet;
static Queue* incoming_packets;
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

bool checkRoundValid(Packet* p)
{
	//get the next packet to send
	Packet* next_transmitter_p = showNextPacketToTransmit();
	if (next_transmitter_p->finish_time < p->arrival_time.round_val)
		return FALSE;
	else
		return TRUE;
}

/*
calc arrival time for the packet using last round
*/
void calcRound(Packet* p)
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
		p->arrival_time.round_time = last_round.round_time + p->time_delta;
		p->arrival_time.round_val = last_round.round_val + (double)p->time_delta / active_links_weights;	
		
	}	
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

	
	p->finish_time = MAX(last_round.round_val, prev_last_pi) + (double)p->length / packet_flow_weight;
	
}
/*
scan the incoming packets
for each packet :
-calc round (with packet delta)
- showNext packet to transmit - check if here finish time is smaller
- if not - insert to buffer and calc lastpi
- if yes - exist and the packet remain in the queue
*/
void HandleInputPackets()
{
	bool packetHandled = FALSE;
	Packet* packet_pointer = (Packet*)queue_front(incoming_packets);
	while (packet_pointer != NULL)
	{
		//check packet and handle:
		calcRound(packet_pointer);

		if (checkRoundValid(packet_pointer))//check if the next packet leave before
		{
			last_round = packet_pointer->arrival_time;
			calcFinishTime(packet_pointer);//calc last pi
			buffer_write(next_packet);//insert to heap
			//remove from queue
			dequeue(incoming_packets);
			packet_pointer = (Packet*)queue_front(incoming_packets);
		}
		else//can't treat the rest of the packets
			return;
	}
	
}

void transmitPacket(Packet pkt)
{
	char* sadd = inet_ntoa(pkt.net_data.src_addr);
	char* dadd = inet_ntoa(pkt.net_data.dst_addr);
	printf("%lld: %s %hu %s %hu %u\n", time, sadd, pkt.net_data.src_port, dadd, pkt.net_data.dst_port, pkt.length);
	transmitting = pkt.length;
}

bool parsePackets()
{
	char line[INPUT_SIZE];
	bool first_packet = FALSE, packets_arrived = FALSE;

	if (time == 0){
		next_packet = (Packet*)malloc(sizeof(Packet));
		if (fgets(line, INPUT_SIZE, stdin) != NULL) parseLine(next_packet, line); //fill in packet
		else return FALSE;
		next_packet->time_delta = 0;
		first_packet = TRUE;
	}

	while (next_packet->time == time)
	{
		if (!first_packet) next_packet->time_delta = time - last_time;
		packets_arrived = TRUE;
		enqueue(incoming_packets, next_packet);

		next_packet = (Packet*)malloc(sizeof(Packet));
		if (fgets(line, INPUT_SIZE, stdin) != NULL) parseLine(next_packet, line); //fill in packet
		else return FALSE;
	}
	if (packets_arrived) last_time = time;
	return TRUE;
}

int main(void)
{
	bool input;
	Packet* packet_to_transmit = NULL;
	InitFlowBuffer();
	time = 0;
	transmitting = 0;
	incoming_packets = create_queue();

	do
	{
		// handle input at this time
		input = parsePackets();

		HandleInputPackets();

		// handle output
		if (transmitting == 0 && !buffer_isEmpty())
		{
			// begin transmition of next packet
			if (packet_to_transmit != NULL){
				packet_to_transmit = removePacketFromBuffer();
				free(packet_to_transmit);
			}
			packet_to_transmit = showNextPacketToTransmit();
			if (packet_to_transmit != NULL) 
				transmitPacket(*packet_to_transmit);
		}

		// advance time
		time++;
		transmitting--;
		
	} while (!buffer_isEmpty() || input);

	freeFlows();

	return 0;
}