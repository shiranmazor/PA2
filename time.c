// DEPRECATED - DELETE ME

#include "time.h"

Time time_start()
{
	Time t;
	t.value = 0;
	return t;
}

time time_getValue(const Time* t)
{
	return t->value;
}

void time_tick(Time* t)
{
	t->value = t->value + 1;
}