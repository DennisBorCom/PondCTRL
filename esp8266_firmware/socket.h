/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */

#ifndef socket_h
#define socket_h

// external libraries
#include "arduino.h"
#include "sensor.h"

#define SOCKET_MODE_ALWAYS_OFF         0
#define SOCKET_MODE_ALWAYS_ON          1
#define SOCKET_MODE_PH1_LOW            2
#define SOCKET_MODE_PH1_HIGH           3
#define SOCKET_MODE_PH2_LOW            4
#define SOCKET_MODE_PH2_HIGH           5
#define SOCKET_MODE_TEMP1_LOW          6
#define SOCKET_MODE_TEMP1_HIGH         7
#define SOCKET_MODE_TEMP2_LOW          8
#define SOCKET_MODE_TEMP2_HIGH         9
#define SOCKET_MODE_LEVEL_LOW          10
#define SOCKET_MODE_LEVEL_HIGH         11

// Sensor structure
struct Socket {

  // string to hold socket name 
  String name;

  // maximum on time
  unsigned int maxOnTime = 0;

  // last switch timestamp
  unsigned int lastSwitchTimestamp = 0;

  // socket mode
  char mode = SOCKET_MODE_ALWAYS_OFF;

  // bool to hold if socket is overridden
  bool override = false;

  // boolean to hold if socket is affected by feed pause
  bool affectedByFeedPause = false;

  // boolean to hold if socket is affected by maintenance
  bool affectedByMaintenance = false;

  // bool to hold if alarm raised, default false
  bool alarmRaised = false;

  // bool to hold if active
  bool isActive = false;

  // unsigned long to hold the device age in seconds
  unsigned long age = 0;

};

#endif
