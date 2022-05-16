#include "uptime.h"
#include <avr/io.h>
#include <util/atomic.h>


static volatile TimeStamp localTime;

ISR(TIMER0_COMPA_vect)
{
    ++localTime;
}

void initializeClock( void )
{
    cli();

    TCNT0  = 0;//initialize counter value to 0
    TCCR0A = 0x02; //No speacial PWM Mode requested; CTC Mode
    TCCR0B = 0x03; //Prescaler of 1024
    OCR0A  = 250;  //1 Interrupt every ms (16000000/64/250)
    TIMSK0 = 0x2;  //Enable Interrupt on compare

    localTime = 0;
    sei();
}

TimeStamp getCurrentTime( void )
{
    TimeStamp result;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        result = localTime;
    }

    return result;
}
