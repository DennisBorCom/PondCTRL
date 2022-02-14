/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */

#ifndef sensor_h
#define sensor_h

// external libraries
#include "arduino.h"

// definitions
#define SENSOR_ENABLED          1
#define SENSOR_DISABLED         0
#define SENSORSTATE_OK          1
#define SENSORSTATE_LOW         2
#define SENSORSTATE_HIGH        3
#define SENSORSTATE_UNCHANGED   0
#define SENSORSTATE_CHANGED     1

#define NUMBEROFVALUES          50

// Sensor structure
class Sensor {

  public:

    // string to hold sensor name 
    String name;
  
    // booleans to hold alarm and enabled/disabled state
    bool alarmRaised = false;
    bool enabled = false;
    bool isSwitch = false;

    // unsigned ints for user specified boundary values
    unsigned int lowValue = 0;
    unsigned int highValue = 0;
    unsigned int lowAlarm = 0;
    unsigned int highAlarm = 0;
    unsigned int threshold = 0;

    // state parameters to handle socket switching
    char state = SENSORSTATE_OK;
    char stateChanged = SENSORSTATE_CHANGED;

    // public member functions
    void setValue(unsigned int value);
    unsigned int getValue();

  private:

    // array of unsigned ints to store values to average
    unsigned int values[NUMBEROFVALUES];
    
    // unsigned int for the number of values stored
    unsigned int valuesStored = 0;

};

#endif
