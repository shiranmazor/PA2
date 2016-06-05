#ifndef _TYPES_H_
#define _TYPES_H_
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
	double round_time;
} Round;

typedef struct Packet
{
	unsigned long long time;
	Net* net_data;
	unsigned int length;
	long weight;		// if -1 no weight was defined - optional field
	double round_val;   // represent packet round time
	double finish_time; // represent last(p(i))
} Packet;

#endif
