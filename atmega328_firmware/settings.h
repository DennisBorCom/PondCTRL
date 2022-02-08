#ifndef settings_h
#define settings_h

#include "socket.h"
#include "sensor.h"

#define DEFAULT_PH1_LOW                           760
#define DEFAULT_PH1_HIGH                          840
#define DEFAULT_PH1_THRESHOLD                     10
#define DEFAULT_PH1_ALARM_LOW                     750
#define DEFAULT_PH1_ALARM_HIGH                    850
#define DEFAULT_PH2_LOW                           760
#define DEFAULT_PH2_HIGH                          840
#define DEFAULT_PH2_THRESHOLD                     10
#define DEFAULT_PH2_ALARM_LOW                     750
#define DEFAULT_PH2_ALARM_HIGH                    850
#define DEFAULT_TEMP1_LOW                         2300
#define DEFAULT_TEMP1_HIGH                        2500
#define DEFAULT_TEMP1_THRESHOLD                   50
#define DEFAULT_TEMP1_ALARM_LOW                   2290
#define DEFAULT_TEMP1_ALARM_HIGH                  2510
#define DEFAULT_TEMP2_LOW              /*23.0*/   1500
#define DEFAULT_TEMP2_HIGH                        2000
#define DEFAULT_TEMP2_THRESHOLD                   50
#define DEFAULT_TEMP2_ALARM_LOW        /*22.9*/   1490
#define DEFAULT_TEMP2_ALARM_HIGH       /*25.1*/   2050
#define DEFAULT_LEVEL_ALARM_LOW                   0
#define DEFAULT_LEVEL_ALARM_HIGH                  0
#define DEFAULT_SOCKET1_MODE         /*temp1L*/   SOCKET_MODE_TEMP2_LOW       // default: heater
#define DEFAULT_SOCKET2_MODE         /*temp2L*/   SOCKET_MODE_TEMP2_HIGH      // default: chiller
#define DEFAULT_SOCKET3_MODE                      SOCKET_MODE_LEVEL_HIGH      // default: circulation pump 
#define DEFAULT_SOCKET4_MODE                      SOCKET_MODE_LEVEL_LOW       // default: auto top off
#define DEFAULT_SOCKET5_MODE                      SOCKET_MODE_ALWAYS_ON       // default: always on
#define DEFAULT_SOCKET1_AFFECTEDBYFEEDPAUSE       SOCKET_UNAFFECTED
#define DEFAULT_SOCKET2_AFFECTEDBYFEEDPAUSE       SOCKET_UNAFFECTED
#define DEFAULT_SOCKET3_AFFECTEDBYFEEDPAUSE       SOCKET_AFFECTED
#define DEFAULT_SOCKET4_AFFECTEDBYFEEDPAUSE       SOCKET_UNAFFECTED
#define DEFAULT_SOCKET5_AFFECTEDBYFEEDPAUSE       SOCKET_UNAFFECTED
#define DEFAULT_SOCKET1_MAXONTIME                 0
#define DEFAULT_SOCKET2_MAXONTIME                 0
#define DEFAULT_SOCKET3_MAXONTIME                 0
#define DEFAULT_SOCKET4_MAXONTIME                 15   // default: 15 - if ato pump runs longer, there may be a leakage
#define DEFAULT_SOCKET5_MAXONTIME                 0
#define DEFAULT_SENSOR1_ENABLED                   SENSOR_ENABLED
#define DEFAULT_SENSOR2_ENABLED      /*ON*/       SENSOR_DISABLED
#define DEFAULT_SENSOR3_ENABLED      /*ON*/       SENSOR_DISABLED
#define DEFAULT_SENSOR4_ENABLED                   SENSOR_ENABLED
#define DEFAULT_SENSOR5_ENABLED                   SENSOR_ENABLED
#define DEFAULT_PH1_NAME                          { 112, 72, 32, 115, 101, 110, 115, 111, 114, 32, 35, 49, 32, 32, 32, 32 }          // pH sensor #1
#define DEFAULT_PH2_NAME                          { 112, 72, 32, 115, 101, 110, 115, 111, 114, 32, 35, 50, 32, 32, 32, 32 }          // pH sensor #2
#define DEFAULT_TEMP1_NAME                        { 116, 101, 109, 112, 32, 115, 101, 110, 115, 111, 114, 32, 35, 49, 32, 32 }       // temp sensor #1
#define DEFAULT_TEMP2_NAME                        { 116, 101, 109, 112, 32, 115, 101, 110, 115, 111, 114, 32, 35, 50, 32, 32 }       // temp sensor #2
#define DEFAULT_LEVEL_NAME                        { 108, 101, 118, 101, 108, 32, 115, 119, 105, 116, 99, 104, 32, 32, 32, 32 }       // level switch
#define DEFAULT_SOCKET1_NAME                      { 104, 101, 97, 116, 101, 114, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32 }            // heater
#define DEFAULT_SOCKET2_NAME                      { 99, 104, 105, 108, 108, 101, 114, 32, 32, 32, 32, 32, 32, 32, 32, 32 }           // chiller
#define DEFAULT_SOCKET3_NAME                      { 99, 105, 114, 99, 117, 108, 97, 116, 105, 111, 110, 32, 112, 117, 109, 112 }     // circulation pump 
#define DEFAULT_SOCKET4_NAME                      { 97, 117, 116, 111, 32, 116, 111, 112, 32, 111, 102, 102, 32, 32, 32, 32 }        // auto top off
#define DEFAULT_SOCKET5_NAME                      { 97, 108, 119, 97, 121, 115, 32, 111, 110, 32, 32, 32, 32, 32, 32, 32 }           // always on 

#endif
