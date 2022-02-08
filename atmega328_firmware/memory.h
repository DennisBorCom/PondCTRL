#ifndef memory_h
#define memory_h

#include "arduino.h"
#include "socket.h"
#include "sensor.h"

// eeprom addresses
#define EEPROM_SENSOR1_ENABLED                 0       // 1 byte: 1 = enabled, 0 = disabled
#define EEPROM_SENSOR2_ENABLED                 1       // 1 byte: 1 = enabled, 0 = disabled  
#define EEPROM_SENSOR3_ENABLED                 2       // 1 byte: 1 = enabled, 0 = disabled
#define EEPROM_SENSOR4_ENABLED                 3       // 1 byte: 1 = enabled, 0 = disabled
#define EEPROM_SENSOR5_ENABLED                 4       // 1 byte: 1 = enabled, 0 = disabled

#define EEPROM_PH1_4_CALIBRATION               140       // 2 byte: adc value, measured at pH4 calibration - byte one is integral part of decimal value, byte two is the fractional part of decimal value
#define EEPROM_PH1_7_CALIBRATION               142     // 2 byte: adc value, measured at pH7 calibration - byte one is integral part of decimal value, byte two is the fractional part of decimal value
#define EEPROM_PH1_LOW                         10
#define EEPROM_PH1_HIGH                        12
#define EEPROM_PH1_THRESHOLD                   14
#define EEPROM_PH1_ALARM_LOW                   16       // 2 bytes    
#define EEPROM_PH1_ALARM_HIGH                  18
#define EEPROM_PH1_NAME                        20       // 16 bytes

#define EEPROM_PH2_4_CALIBRATION               144      // 2 byte: adc value, measured at pH4 calibration - byte one is integral part of decimal value, byte two is the fractional part of decimal value
#define EEPROM_PH2_7_CALIBRATION               146     // 2 byte: adc value, measured at pH7 calibration - byte one is integral part of decimal value, byte two is the fractional part of decimal value
#define EEPROM_PH2_LOW                         36
#define EEPROM_PH2_HIGH                        38
#define EEPROM_PH2_THRESHOLD                   40
#define EEPROM_PH2_ALARM_LOW                   42
#define EEPROM_PH2_ALARM_HIGH                  44
#define EEPROM_PH2_NAME                        46

#define EEPROM_TEMP1_LOW                       62
#define EEPROM_TEMP1_HIGH                      64
#define EEPROM_TEMP1_THRESHOLD                 66
#define EEPROM_TEMP1_ALARM_LOW                 68
#define EEPROM_TEMP1_ALARM_HIGH                70
#define EEPROM_TEMP1_NAME                      72

#define EEPROM_TEMP2_LOW                       88
#define EEPROM_TEMP2_HIGH                      90
#define EEPROM_TEMP2_THRESHOLD                 92
#define EEPROM_TEMP2_ALARM_LOW                 94
#define EEPROM_TEMP2_ALARM_HIGH                96
#define EEPROM_TEMP2_NAME                      98

/* level switch has no low/high/threshold: 6 bytes spacing to make eeprom offset calculation possible */

#define EEPROM_LEVEL_ALARM_LOW                 120       // level alarm is : 1 = alarm, 0 = no alarm
#define EEPROM_LEVEL_ALARM_HIGH                122
#define EEPROM_LEVEL_NAME                      124


#define EEPROM_SOCKET1_OPERATION               148       // 1 byte
#define EEPROM_SOCKET2_OPERATION               149
#define EEPROM_SOCKET3_OPERATION               150
#define EEPROM_SOCKET4_OPERATION               151
#define EEPROM_SOCKET5_OPERATION               152
#define EEPROM_SOCKET1_AFFECTEDBYFEEDPAUSE     153       // 1 byte
#define EEPROM_SOCKET2_AFFECTEDBYFEEDPAUSE     154
#define EEPROM_SOCKET3_AFFECTEDBYFEEDPAUSE     155
#define EEPROM_SOCKET4_AFFECTEDBYFEEDPAUSE     156
#define EEPROM_SOCKET5_AFFECTEDBYFEEDPAUSE     157
#define EEPROM_SOCKET1_MAXONTIME               158       // 2 bytes
#define EEPROM_SOCKET2_MAXONTIME               160
#define EEPROM_SOCKET3_MAXONTIME               162
#define EEPROM_SOCKET4_MAXONTIME               164
#define EEPROM_SOCKET5_MAXONTIME               166
#define EEPROM_SOCKET1_NAME                    168      // 16 bytes
#define EEPROM_SOCKET2_NAME                    184
#define EEPROM_SOCKET3_NAME                    200
#define EEPROM_SOCKET4_NAME                    216
#define EEPROM_SOCKET5_NAME                    232      

//#define EEPROM_ALARM_MUTE                      248      
//#define EEPROM_ALARM_AUTORESET                 249      

#define EEPROM_TIMER1                          250      // 12 bytes: 24 uur * 4 schakelmomenten / 8 bits
#define EEPROM_TIMER2                          262
#define EEPROM_TIMER3                          274
#define EEPROM_TIMER4                          286
#define EEPROM_TIMER5                          298     

#define EEPROM_TEMP1_CALIBRATION_POSITIVE      310      // 2 byte
#define EEPROM_TEMP1_CALIBRATION_NEGATIVE      312
#define EEPROM_TEMP2_CALIBRATION_POSITIVE      314
#define EEPROM_TEMP2_CALIBRATION_NEGATIVE      316       /// NEXT: 318

float readFloatFromMemory(int memoryOffset);
unsigned int readUnsignedIntFromMemory(int memoryOffset);
void writeFloatToMemory(int memoryOffset, float value);
void writeUnsignedIntToMemory(int memoryOffset, int value);
void writeByteToMemory(int memoryOffset, char value);
char readByteFromMemory(int memoryOffset);
void hardReset();
void writeBytesToMemory(int memoryOffset, byte bytes[], unsigned int byteLength);
void readBytesFromMemory(int memoryOffset, byte bytes[], unsigned int byteLength);
void updateEEPROM(Sensor sensors[5], Socket sockets[5]);

#endif
