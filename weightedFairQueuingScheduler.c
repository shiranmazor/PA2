#pragma warning(disable: 4996)
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "weightedFairQueuingScheduler.h"


static unsigned long long time;
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
void parseLine(Packet* p, char* line)
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
	if (next_transmitter_p->finish_time < p->round_val)
		return FALSE;
	else
		return TRUE;
}

/*
calc arrival time for the packet using last round
*/
void calcRound(Packet* p)
{
	long active_links_weights = buffer_getTotalWeight(TRUE); //get weight of the virtual buffer
	if (active_links_weights == 0) 
		p->round_val = p->time;
	else
		p->round_val = last_round.round_val + (double)(p->time - last_round.round_time) / active_links_weights;
}

/*
calc last_pi of current packet 
*/
void calcFinishTime(Packet* p)
{	
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
		int weight = 0;
		if (p->weight > 0)
			weight = p->weight;
		else
			weight = packet_flow->weight;


		p->finish_time = MAX(last_round.round_val, packet_flow->last) + (double)p->length / weight;
		packet_flow->last = p->finish_time;
	}
	
}

/*
check if our real time heap is empty and there is nothing to transmit
*/
bool buffer_isIdle()
{
	if (transmitting == 0 && buffer_isEmpty(FALSE) == TRUE)
		return TRUE;
	return FALSE;
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
		calcRound(packet_pointer);
		//while current round is bigger then last_pi of virtual packet
		//remove the virtual packet from buffer
		while (checkRoundValid(packet_pointer) == FALSE)
		{
			//extract real delta and
			long weights = buffer_getTotalWeight(TRUE);
			Packet* next_virtual_p = removePacketFromBuffer(TRUE);
			double x = (next_virtual_p->finish_time - last_round.round_val)*weights;
			
			//update last round
			last_round.round_val = next_virtual_p->finish_time;
			last_round.round_time += x;

			free(next_virtual_p);

			//calc round again with correct weights:
			calcRound(packet_pointer);
		}
		//packet round is valid, update round global 
		last_round.round_time = packet_pointer->time;
		last_round.round_val = packet_pointer->round_val;
		
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
	printf("round_val %f ", pkt.round_val);
	printf("f_time %f ", pkt.finish_time);
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
		//next_packet->time_delta = 0;
		first_packet = TRUE;
	}

	while (next_packet->time == time)
	{
		//Packet* pp;
		//if (!first_packet) next_packet->time_delta = time - last_time;
		packets_arrived = TRUE;
		enqueue(incoming_packets, next_packet);
		
		next_packet = (Packet*)malloc(sizeof(Packet));
		if (fgets(line, INPUT_SIZE, stdin) != NULL)
			parseLine(next_packet, line); //fill in packet
			
		else
			return FALSE;
	}
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
		//Packet* pp;
		// handle input at this time
		if (input) 
			input = parsePackets();

		//debug
		HandleInputPackets();

		// handle output
		if (transmitting == 0 && !buffer_isEmpty(FALSE))
		{
			//remove packet from real heap
			//pp = getPacketFromBuffer();
			packet_to_transmit = removePacketFromBuffer(FALSE);
			transmitPacket(*packet_to_transmit);
			free(packet_to_transmit);
			//pp = getPacketFromBuffer();
			//packet_to_transmit = NULL;
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