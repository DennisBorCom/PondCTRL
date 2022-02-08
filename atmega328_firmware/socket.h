#ifndef socket_h
#define socket_h

#include "sensor.h"
#include "arduino.h"

//#define SOCKET_ON 1
//#define SOCKET_OFF 0

#define SOCKET_AFFECTED 1
#define SOCKET_UNAFFECTED 0

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

struct Socket {
  char pinNumber = 0;
  char mode = SOCKET_MODE_ALWAYS_OFF;
  unsigned int maxOnTime = 0;
  unsigned int lastSwitchTimestamp = 0;
  byte name[16];
  bool override = false;
  bool affectedByFeedPause = false;
  bool alarmRaised = false;
};

void setSocketSettings(Socket &socket, char pinNumber, char socketMode, bool socketAffectedByFeedPause, unsigned int socketMaxOnTime, byte name[16]);
void switchSockets(Sensor sensors[5], Socket sockets[5], bool feedPause);
void setPinHighOrLow(Socket &socket, Sensor &sensor, int modeSet, int requiredState, bool feedPause);
void detectSocketWarnings(Socket sockets[5]);

#endif
