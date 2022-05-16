#include "sensor.h"

#include <avr/io.h>
#include <Arduino.h>
#include <string.h>

#include <stdint.h>

static int sensorHistory[4][16];
static unsigned historyWritePointer_;
/*min 270, max 650, vcc: 3.3*/

void initMoistureSensors( void )
{
    memset(&sensorHistory[0][0],0,sizeof(sensorHistory));
}

static void readSensor(unsigned const sensor)
{
     int value = analogRead(A0 + sensor);

     value = ((value - 270) * 27) / 10;

     if(value < 0) value = 0;
     if(value > 1023) value = 1023;
    
     sensorHistory[sensor][historyWritePointer_] = value;
}


void updateSensors( void )
{
    historyWritePointer_ = historyWritePointer_ == 0 ? 15 : historyWritePointer_ - 1;

    for(unsigned i=0; i< 4; ++i)
    {
        readSensor(i);
    }
}

int getMesurement(unsigned const sendor, unsigned const age)
{
    return sensorHistory[sendor][(historyWritePointer_ + age) % 16];
}
