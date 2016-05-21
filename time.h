/*
Time module
Provides an API to keep track of discrete time axis
*/
#ifndef _TIME_H_
#define _TIME_H_

typedef unsigned long long time;

typedef struct Time
{
	time value;		// value of current time
} Time;

// initializes the time module
Time time_start();

// moves the time one unit forward in time
void time_tick(Time* t);

#endif