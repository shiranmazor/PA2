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
	char * net = NULL;
	p->weight = -1; // weight default
	p->net_data = malloc(sizeof(Net));
	pch = strtok(line, " ");
	while (pch != NULL)
	{
		if (parameterIndex == 0)
			p->time = atol(pch);
		if (parameterIndex == 1)
			p->net_data->src_addr.S_un.S_addr = inet_addr(pch);
		if (parameterIndex == 2)
			p->net_data->src_port = atoi(pch);
		if (parameterIndex == 3)
			p->net_data->dst_addr.S_un.S_addr = inet_addr(pch);
		if (parameterIndex == 4)
			p->net_data->dst_port = atoi(pch);
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
	//get the next packet to send from virtual heap
	Packet* next_transmitter_p = showNextPacketToTransmit(TRUE);
	if (next_transmitter_p == NULL)
		return TRUE;
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
	long active_links_weights = 0;


	active_links_weights = buffer_getTotalWeight(TRUE);//get weight of the virtual buffer
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
	//get relevent flow and extract the last_p(i-1) of the previous packet from the virtual
	Flow* packet_flow = findFlow(p,TRUE);
	if (packet_flow == NULL)
	{
		int weight = 1;
		if (p->weight > 0)
			weight = p->weight;

		p->finish_time = MAX(last_round.round_val, 0.0) + (double)p->length / weight;
	}
	else
	{
		int prev_last_pi = 0;
		if (packet_flow->packets->count != 0)
			prev_last_pi = ((Packet*)queue_front(packet_flow->packets))->finish_time;

		int weight = 0;
		if (p->weight > 0)
			weight = p->weight;
		else
			weight = packet_flow->weight;

		p->finish_time = MAX(last_round.round_val, prev_last_pi) + (double)p->length / weight;
	}

	
}

/*
check if our real time heap is empty and there is nothing to transmit
*/
bool buffer_isIdle()
{
	if (transmitting == 0 && buffer_isEmpty(FALSE) == TRUE)
		return TRUE;
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
	while (packet_pointer)
	{
		//check packet and handle:
		if (buffer_isIdle() == TRUE && buffer_isEmpty(TRUE) == FALSE)//if we finished transmit this packet in real time, remove from the virtual time
		{
			//debug
			Packet* pp = getPacketFromBuffer();
			Packet* last_packet = removePacketFromBuffer(TRUE);//departure time of current packet has arrived
			free(last_packet);
			last_packet = NULL;
		}
		calcRound(packet_pointer);
		//while current round is bigger then last_pi of virtual packet
		//remove the virtual packet from buffer
		while (checkRoundValid(packet_pointer) == FALSE)
		{
			Packet* last_packet;
			//check if the packet still in real flow
			Packet* real_p = showNextPacketToTransmit(FALSE);
			Packet* virtual_p = showNextPacketToTransmit(TRUE);
			if (real_p != NULL)
			{
				if (real_p->time == virtual_p->time)
					return;
			}
			last_packet = removePacketFromBuffer(TRUE);//departure time of current packet has arrived
			free(last_packet);
			last_packet = NULL;
			//calc round again with correct weights:
			calcRound(packet_pointer);
		}
		//packet round is valid, update round global 
		last_round = packet_pointer->arrival_time;
		calcFinishTime(packet_pointer);//calc last pi
		//insert new packet to both virtual heap and real time heap
		buffer_write(packet_pointer, TRUE);
		buffer_write(packet_pointer,FALSE);	
		dequeue(incoming_packets);
		free(packet_pointer);//after performing dequeue we are free the packet
		packet_pointer = (Packet*)queue_front(incoming_packets);

	}
	
}

void transmitPacket(Packet pkt)
{
	char* add = inet_ntoa(pkt.net_data->src_addr);
	printf("%lld: %lld %s %hu ", time, pkt.time, add, pkt.net_data->src_port);
	add = inet_ntoa(pkt.net_data->dst_addr);
	if (pkt.weight != -1)
		printf("%s %hu %u    %ld\n", add, pkt.net_data->dst_port, pkt.length, pkt.weight);
	else
		printf("%s %hu %u\n", add, pkt.net_data->dst_port, pkt.length);
	transmitting = pkt.length;
}

bool parsePackets()
{
	char line[INPUT_SIZE];
	bool first_packet = FALSE, packets_arrived = FALSE;

	if (time == 0)
	{
		next_packet = (Packet*)malloc(sizeof(Packet));
		if (fgets(line, INPUT_SIZE, stdin) != NULL)
			parseLine(next_packet, line); //fill in packet
		else return FALSE;
		next_packet->time_delta = 0;
		first_packet = TRUE;
	}

	while (next_packet->time == time)
	{
		Packet* pp;
		if (!first_packet) next_packet->time_delta = time - last_time;
		packets_arrived = TRUE;
		enqueue(incoming_packets, next_packet);
		
		next_packet = (Packet*)malloc(sizeof(Packet));
		if (fgets(line, INPUT_SIZE, stdin) != NULL)
			parseLine(next_packet, line); //fill in packet
			
		else
			return FALSE;
	}
	if (packets_arrived) last_time = time;
	return TRUE;
}

int main(void)
{
	bool input = TRUE;
	Packet* packet_to_transmit = NULL;
	InitFlowBuffer();
	time = 0;
	transmitting = 0;
	incoming_packets = create_queue();

	do
	{
		Packet* pp;
		// handle input at this time
		if (input) 
			input = parsePackets();

		//debug
		HandleInputPackets();

		// handle output
		if (transmitting == 0 && !buffer_isEmpty(FALSE))
		{
			//remove packet from real heap
			pp = getPacketFromBuffer();
			packet_to_transmit = removePacketFromBuffer(FALSE);
			transmitPacket(*packet_to_transmit);
			free(packet_to_transmit);
			pp = getPacketFromBuffer();
			packet_to_transmit = NULL;
		}

		// advance time
		time++;
		if (transmitting > 0)
			transmitting--;
		
	} while (!buffer_isEmpty(FALSE) || input || transmitting != 0 || !queue_isEmpty(incoming_packets));

	freeFlows();
	queue_free(incoming_packets);
	free(next_packet);
	return 0;
}