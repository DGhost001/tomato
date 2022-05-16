#ifndef UPTIME_H
#define UPTIME_H

#include <stdint.h>

typedef uint64_t TimeStamp;

void initializeClock( void );
TimeStamp getCurrentTime( void );

#endif // UPTIME_H
