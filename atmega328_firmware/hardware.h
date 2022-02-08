
#ifndef hardware_h
#define hardware_h

#include "socket.h"
#include "arduino.h"
// constant definitions

// atmega pins
#define PIN_BUTTON                5
#define PIN_LED                   10
#define PIN_LEVEL                 12
#define PIN_PH1                   A3
#define PIN_PH2                   A1
#define PIN_SOCKET1               2
#define PIN_SOCKET2               3
#define PIN_SOCKET3               4
#define PIN_SOCKET4               6
#define PIN_SOCKET5               7
#define PIN_SPEAKER               11
#define PIN_TEMP1                 13
#define PIN_TEMP2                 9

// function definitions
void initializeHardware();

#endif
