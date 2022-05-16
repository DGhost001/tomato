#ifndef SENSOR_H
#define SENSOR_H

void initMoistureSensors( void );
void updateSensors( void );

int getMesurement(const unsigned sendor, const unsigned age);


#endif // SENSOR_H
