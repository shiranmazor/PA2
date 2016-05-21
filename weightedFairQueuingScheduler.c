#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include "weightedFairQueuingScheduler.h"

static Time t;

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
bool HandleInputPacket(char* inputLine)
{
	Packet* p = NULL;
	p = (Packet*)malloc(sizeof(Packet));
	if (p == NULL)
	{
		printf("packet allocation failed");
		return NULL;
	}

	
}



void InitScheduler()
{
	t = time_start();
}


int main(void)
{
	char line[INPUT_SIZE];
	bool firstLine = TRUE;
	while (fgets(line, INPUT_SIZE, stdin) != NULL)
	{
		if (firstLine)
		{
			InitScheduler();
			firstLine = FALSE;
		}
		HandleInputPacket(line);
		printf("%s\n", line);
	}
	return 0;
}