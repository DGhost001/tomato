#include "ansi.h"
#include "uptime.h"
#include "sensor.h"
#include "pump.h"
#include "menu.h"
#include "settings.h"

#define SAMPLE_INTERVAL 60000

static TimeStamp nextPrint_;
int thresholds_[4];




void setup() {
  Serial.begin(19200);
  Serial.println("Starting...");
  nextPrint_ = 0;
  initMoistureSensors();
  initPumps();
  initializeClock();

  for(unsigned i = 0; i<4; ++i) {
    pumpSetTurnOnTime(i, 10);   //10 sec on
    pumpSetTurnOffTime(i, 60);  //60 sec off
    thresholds_[i] = 512;
  }

  Settings *s = loadSettings();

  if(s) //Only load settings if they are valid
  {
      for(unsigned i = 0; i< 4; ++i){
          pumpSetTurnOnTime(i,s->pumpSettings[i].turnOnTime);
          pumpSetTurnOffTime(i,s->pumpSettings[i].turnOffTime);
          thresholds_[i] = s->pumpSettings[i].threshold;
      }
  } else {
    Serial.println("No Valid Settings found, using defaults!");
  }

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
}

void loop() {

    TimeStamp now = getCurrentTime();   
    if( now > nextPrint_)
    {
        nextPrint_ += SAMPLE_INTERVAL;
        
        updateSensors();
        
        for(unsigned i= 0; i< 4; ++i) {
          int const sensor = getMesurement(i,0);
          Serial.print(sensor);
          Serial.print(';');  

          if(sensor > thresholds_[i]) {
            Serial.print(pumpTurnOn(i));
          } else {
            Serial.print(0);
          }
          Serial.print(';');  
        }
        Serial.println();        
    }

    if(Serial.available())
    {
       char const c = Serial.read();

       if('m' == c) {
           for(unsigned i=0; i< 4; ++i) {
               pumpTurnOff(i); //Turn all pumps off to avoid flooding while beeing in the config menu
           }
           enterMenu();
           nextPrint_ = getCurrentTime();
       }
    }    
    pumpLoopCallBack();
}
