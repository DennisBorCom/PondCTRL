/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */
#ifndef i2c_h
#define i2c_h

// external libraries
#include "arduino.h"
#include <Wire.h> 

// own libraries
#include "sensor.h"
#include "socket.h"
#include "memory.h"

// the communication protocol works by sending a command byte and an
// 16 byte data array (may be empty, but required, Pond[CTRL]™ only works
// with 17 byte requests). The answer is always a 16-byte data array.

// communication protocol command bytes. the list below contains only the 
// commands used by the WiFi module. the are many more command bytes which
// can be used by clients, and passed through to Pond[CTRL]™ by using
// web queries. see WebServer::query for more details.

#define I2C_SET_ALARM_ON                             9
#define I2C_SET_ALARM_OFF                           10
#define I2C_GET_SENSOR_VALUES                       53
#define I2C_GET_SENSOR_PH1_NAME                     54
#define I2C_GET_SENSOR_PH2_NAME                     55
#define I2C_GET_SENSOR_TEMP1_NAME                   56 
#define I2C_GET_SENSOR_TEMP2_NAME                   57
#define I2C_GET_SENSOR_LEVEL_NAME                   58
#define I2C_GET_SENSOR_PH1_SETTINGS                 59
#define I2C_GET_SENSOR_PH2_SETTINGS                 60
#define I2C_GET_SENSOR_TEMP1_SETTINGS               61 
#define I2C_GET_SENSOR_TEMP2_SETTINGS               62
#define I2C_GET_SENSOR_LEVEL_SETTINGS               63
#define I2C_GET_SOCKET1_NAME                        89
#define I2C_GET_SOCKET2_NAME                        90
#define I2C_GET_SOCKET3_NAME                        91
#define I2C_GET_SOCKET4_NAME                        92
#define I2C_GET_SOCKET5_NAME                        93 
#define I2C_GET_SOCKET1_SETTINGS                    84
#define I2C_GET_SOCKET2_SETTINGS                    85
#define I2C_GET_SOCKET3_SETTINGS                    86
#define I2C_GET_SOCKET4_SETTINGS                    87
#define I2C_GET_SOCKET5_SETTINGS                    88 
#define I2C_GET_SPECIAL_INSTRUCTIONS               255
#define I2C_SET_SOCKET1_ON                          99
#define I2C_SET_SENSOR_PH1_NAME                     64
#define I2C_SET_SENSOR_PH1_VALUES                   65
//#define I2C_SET_SOCKET1_NAME                        74
//#define I2C_SET_SOCKET1_VALUES                      79
#define I2C_SET_PH1_CALIBRATION4                    29
#define I2C_SET_PH1_CALIBRATION7                    30
#define I2C_SET_PH1_CALIBRATIONRESET                97
#define I2C_SET_TEMP1_CORRECTION                    95
#define I2C_SET_LED_ON                              11
#define I2C_SET_LED_OFF                             12

// special instructions which can be requested by
// the i2c slave.

#define I2C_SPECIAL_INSTRUCTION_HARD_RESET         112

/**
 * I2C class
 */
class I2C {
  
  private:

    // private memorytools object
    MemoryTools memTools;

    // private function declarations
    String byteArrayToString(byte byteArray[], char length);

  public:

   // public function declarations
   void processCommand(byte command, byte dataTransmitted[16], byte dataReceived[16], Sensor sensors[5], Socket sockets[5]);
   void processCommand(byte command, byte dataTransmitted[16], byte dataReceived[16]);  
   bool checkDataIntegrity(byte data[16]);
  
};

#endif
