#ifndef button_h
#define button_h

#include "hardware.h"
#include "arduino.h"

bool handleButton(Sensor sensors[5], Socket sockets[5], bool &feedPause, byte &specialCommandToWiFiModule);

#endif
