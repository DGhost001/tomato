#ifndef PUMP_H
#define PUMP_H



void initPumps( void );

void pumpSetTurnOnTime(unsigned const pump, unsigned const time);
void pumpSetTurnOffTime(unsigned const pump, unsigned const time);

unsigned pumpGetTurnOnTime(unsigned const pump);
unsigned pumpGetTurnOffTime(unsigned const pump);

void pumpToggleOnOff(unsigned const pump);

int pumpTurnOn(unsigned const pump);
void pumpTurnOff(unsigned const pump);
void pumpLoopCallBack( void );
int pumpStatus(unsigned const pump);

#endif // PUMP_H
