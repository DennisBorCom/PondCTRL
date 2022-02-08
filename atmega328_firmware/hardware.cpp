#include "arduino.h"
#include "hardware.h"
#include "socket.h"



void initializeHardware() {

  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_LEVEL, INPUT);
  pinMode(PIN_PH1, INPUT);
  pinMode(PIN_PH2, INPUT);
  pinMode(PIN_SOCKET1, OUTPUT);
  pinMode(PIN_SOCKET2, OUTPUT);
  pinMode(PIN_SOCKET3, OUTPUT);
  pinMode(PIN_SOCKET4, OUTPUT);
  pinMode(PIN_SOCKET5, OUTPUT);
  pinMode(PIN_SPEAKER, OUTPUT);       
  pinMode(PIN_TEMP1, INPUT);
  pinMode(PIN_TEMP2, INPUT);

  // turn off sockets by default
  digitalWrite(PIN_SOCKET1, LOW);
  digitalWrite(PIN_SOCKET2, LOW);
  digitalWrite(PIN_SOCKET3, LOW);
  digitalWrite(PIN_SOCKET4, LOW);
  digitalWrite(PIN_SOCKET5, LOW);
  digitalWrite(PIN_SPEAKER, LOW);
}
