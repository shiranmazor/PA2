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

typedef IN_ADDR Ip;
typedef unsigned short Port;
typedef unsigned char bool;
typedef unsigned char bit;
typedef unsigned char byte;
typedef unsigned long long ulong;

typedef struct Net
{
	Ip src_addr;
	Port src_port;
	Ip dst_addr;
	Port dst_port;
} Net;


typedef struct Packet
{
	time time;
	Net net_data;
	unsigned int length;
	long weight;	// defaults to 1
} Packet;



#endif
