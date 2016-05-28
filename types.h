#ifndef _TYPES_H_
#define _TYPES_H_
#include <WinSock2.h>
#include "time.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_WEIGHT	1
#define TRUE	1
#define FALSE	0
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

typedef IN_ADDR Ip;
typedef unsigned short Port;
typedef unsigned char bool;
typedef unsigned char bit;
typedef unsigned char byte;

typedef struct Net
{
	Ip src_addr;
	Port src_port;
	Ip dst_addr;
	Port dst_port;
} Net;

typedef struct Round
{
	double round_val;
	long round_time;
} Round;

typedef struct Packet
{
	unsigned long long time;
	Net net_data;
	unsigned int length;
	long weight;	//if zero , no weight has  arrived - optional field
	Round arrival_time;//represent packet round time
	double finish_time;//represent last_pi
	unsigned long long time_delta;
} Packet;





#endif
