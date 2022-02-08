#ifndef sensor_h
#define sensor_h

#include "arduino.h"
#include "convert.h"

#define SENSOR_ENABLED 1
#define SENSOR_DISABLED 0

#define SENSORSTATE_OK 1
#define SENSORSTATE_LOW 2
#define SENSORSTATE_HIGH 3

#define SENSORSTATE_UNCHANGED 0
#define SENSORSTATE_CHANGED 1

#define SENSOR_PH1  0
#define SENSOR_PH2  1
#define SENSOR_TEMP1  2
#define SENSOR_TEMP2  3
#define SENSOR_LEVEL  4

struct Sensor {
  bool alarmRaised = false;
  bool onOffOnly = false;
  char enabled = true;
 
  
  unsigned int value = 0;

   // array of unsigned ints to store values to average
 // unsigned int values[60];
  
  // unsigned int for the number of values stored
  //unsigned int valuesStored = 0;

  unsigned int alarmLow;
  //unsigned int alarmLowUnsignedInt;
  unsigned int alarmHigh;
  //unsigned int alarmHighUnsignedInt;
  unsigned int lowValue;
  //unsigned int lowValueUnsignedInt;
  unsigned int highValue;
  //unsigned int highValueUnsignedInt;
  unsigned int threshold;
  //unsigned int thresholdUnsignedInt;
  char state = SENSORSTATE_OK;
  char stateChanged = SENSORSTATE_UNCHANGED;
  byte name[16];
  int correctionValue = 0;
  unsigned int adcPH4 = 0;
  unsigned int adcPH7 = 0;
};

void setState(Sensor &sensor, char state);
void setSensorValues(Sensor &sensor, unsigned int lowValue, unsigned int highValue, unsigned int threshold, byte name[16], unsigned int alarmLow, unsigned int alarmHigh, int correctionValue, unsigned int adcPH4, unsigned int adcPH7);
void calculateSensorState(Sensor &sensor);
void setValue(Sensor &sensor, unsigned int value);
unsigned int getValue(Sensor sensor);

#endif
