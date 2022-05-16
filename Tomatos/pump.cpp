#include "pump.h"
#include "uptime.h"

#include <avr/io.h>
#include <Arduino.h>

#include <string.h>

struct Pumps {
    TimeStamp nextOffTime;
    TimeStamp nextOnTime;
    unsigned state;
    unsigned turnOffIntervall_;
    unsigned turnOnIntervall_;
} pumps[4];


void initPumps( void )
{
    for(unsigned i=0; i<4; ++i) {
        pinMode(8+i,OUTPUT);
        digitalWrite(8+i, HIGH);
    }

    memset(&pumps[0],0, sizeof(pumps));
}

void pumpSetTurnOnTime(unsigned const pump, unsigned const time)
{
    pumps[pump].turnOnIntervall_ = time;
}

void pumpSetTurnOffTime(unsigned const pump, unsigned const time)
{
    pumps[pump].turnOffIntervall_ = time;
}

int pumpTurnOn(unsigned const pump)
{
    TimeStamp now = getCurrentTime();
    if(!pumps[pump].state && now > pumps[pump].nextOnTime)
    {
        pumps[pump].state = 1;
        pumps[pump].nextOffTime = now + static_cast<TimeStamp>(pumps[pump].turnOnIntervall_)*1000ull;
        digitalWrite(8+pump,LOW);
    }

    return pumps[pump].state;
}

void pumpTurnOff(unsigned const pump)
{
    TimeStamp now = getCurrentTime();
    pumps[pump].state = 0;
    digitalWrite(8+pump,HIGH);
    pumps[pump].nextOnTime = now + static_cast<TimeStamp>(pumps[pump].turnOffIntervall_)*1000ull;
}

void pumpLoopCallBack( void )
{
    TimeStamp now = getCurrentTime();

    for(unsigned i=0; i<4; ++i)
    {
        if(now > pumps[i].nextOffTime && pumps[i].state)
        {
            pumpTurnOff(i);
        }
    }
}

int pumpStatus(const unsigned pump)
{
    return pumps[pump].state;
}

unsigned pumpGetTurnOnTime(unsigned const pump)
{
    return pumps[pump].turnOnIntervall_;
}

unsigned pumpGetTurnOffTime(unsigned const pump)
{
    return pumps[pump].turnOffIntervall_;
}

void pumpToggleOnOff(const unsigned pump)
{
    if(!pumps[pump].state)
    {
        pumps[pump].state = 1;
        digitalWrite(8+pump,LOW);
    } else {
        pumps[pump].state = 0;
        digitalWrite(8+pump,HIGH);
    }
}
