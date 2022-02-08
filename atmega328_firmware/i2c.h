#ifndef i2c_h
#define i2c_h

#include "arduino.h"
#include "hardware.h"
#include "sensor.h"
#include "socket.h"
#include "memory.h"

#define I2C_OFF                      0
#define I2C_ON                       1

#define I2C_DATATYPE_NONE            0
#define I2C_DATATYPE_UNSIGNEDINT     1
#define I2C_DATATYPE_FLOAT           2
#define I2C_DATATYPE_STRING16        3

/**
 * I2C Commando's (0-255)
 */
#define I2C_GET_DEVICE_ID              1
#define I2C_GET_SENSOR_PH1_VALUE       2
#define I2C_GET_SENSOR_PH1_NAME       54 
#define I2C_GET_SENSOR_PH1_SETTINGS   59
#define I2C_GET_SENSOR_PH2_VALUE       3
#define I2C_GET_SENSOR_PH2_NAME       55
#define I2C_GET_SENSOR_PH2_SETTINGS   60
#define I2C_GET_SENSOR_TEMP1_VALUE     4
#define I2C_GET_SENSOR_TEMP1_NAME     56
#define I2C_GET_SENSOR_TEMP1_SETTINGS 61
#define I2C_GET_SENSOR_TEMP2_VALUE     5
#define I2C_GET_SENSOR_TEMP2_NAME     57
#define I2C_GET_SENSOR_TEMP2_SETTINGS 62
#define I2C_GET_SENSOR_LEVEL_VALUE     6
#define I2C_GET_SENSOR_LEVEL_NAME     58 
#define I2C_GET_SENSOR_LEVEL_SETTINGS 63 
#define I2C_GET_SENSOR_VALUES         53 
#define I2C_SET_ALARM_ON               9
#define I2C_SET_ALARM_OFF             10
#define I2C_SET_LED_ON                11
#define I2C_SET_LED_OFF               12


//#define I2C_SET_SOCKET1_FEEDPAUSE     33
//#define I2C_SET_SOCKET2_FEEDPAUSE     34
//#define I2C_SET_SOCKET3_FEEDPAUSE     35
//#define I2C_SET_SOCKET4_FEEDPAUSE     36
//#define I2C_SET_SOCKET5_FEEDPAUSE     37 

//#define I2C_SET_SOCKET1_MAXONTIME     38
//#define I2C_SET_SOCKET2_MAXONTIME     39
//#define I2C_SET_SOCKET3_MAXONTIME     40
//#define I2C_SET_SOCKET4_MAXONTIME     41
//#define I2C_SET_SOCKET5_MAXONTIME     42 

//#define I2C_SET_PH1_LOW               12->gebruikt
//#define I2C_SET_PH1_HIGH              13
//#define I2C_SET_PH1_THRESHOLD         14
#define I2C_SET_PH1_CALIBRATION4      29
#define I2C_SET_PH1_CALIBRATION7      30
//#define I2C_SET_PH1_ALARM_LOW         43
//#define I2C_SET_PH1_ALARM_HIGH        44
#define I2C_SET_PH1_NAME              64 
#define I2C_SET_PH1_VALUES            65 
#define I2C_SET_PH1_CALIBRATIONRESET  97

//#define I2C_SET_PH2_LOW               15
//#define I2C_SET_PH2_HIGH              16
//#define I2C_SET_PH2_THRESHOLD         17
#define I2C_SET_PH2_CALIBRATION4      31
#define I2C_SET_PH2_CALIBRATION7      32
//#define I2C_SET_PH2_ALARM_LOW         45
//#define I2C_SET_PH2_ALARM_HIGH        46 
#define I2C_SET_PH2_NAME              66 
#define I2C_SET_PH2_VALUES            67 
#define I2C_SET_PH2_CALIBRATIONRESET  98  

//#define I2C_SET_TEMP1_LOW             18
//#define I2C_SET_TEMP1_HIGH            19
//#define I2C_SET_TEMP1_THRESHOLD       20
//#define I2C_SET_TEMP1_ALARM_LOW       47
//#define I2C_SET_TEMP1_ALARM_HIGH      48
#define I2C_SET_TEMP1_NAME            68
#define I2C_SET_TEMP1_VALUES          69
#define I2C_SET_TEMP1_CORRECTION      95

//#define I2C_SET_TEMP2_LOW             21
//#define I2C_SET_TEMP2_HIGH            22
//#define I2C_SET_TEMP2_THRESHOLD       23
//#define I2C_SET_TEMP2_ALARM_LOW       49
//#define I2C_SET_TEMP2_ALARM_HIGH      50
#define I2C_SET_TEMP2_NAME            70
#define I2C_SET_TEMP2_VALUES          71
#define I2C_SET_TEMP2_CORRECTION      96  

//#define I2C_SET_LEVEL_ALARM_LOW       51
//#define I2C_SET_LEVEL_ALARM_HIGH      52 
#define I2C_SET_LEVEL_NAME            72
#define I2C_SET_LEVEL_VALUES          73 

#define I2C_SET_SOCKET1_NAME          74 
#define I2C_SET_SOCKET2_NAME          75
#define I2C_SET_SOCKET3_NAME          76
#define I2C_SET_SOCKET4_NAME          77
#define I2C_SET_SOCKET5_NAME          78

#define I2C_GET_SOCKET1_NAME          89
#define I2C_GET_SOCKET2_NAME          90
#define I2C_GET_SOCKET3_NAME          91
#define I2C_GET_SOCKET4_NAME          92
#define I2C_GET_SOCKET5_NAME          93    

#define I2C_SET_SOCKET1_VALUES        79
#define I2C_SET_SOCKET2_VALUES        80
#define I2C_SET_SOCKET3_VALUES        81
#define I2C_SET_SOCKET4_VALUES        82
#define I2C_SET_SOCKET5_VALUES        83   

#define I2C_GET_SOCKET1_SETTINGS      84
#define I2C_GET_SOCKET2_SETTINGS      85
#define I2C_GET_SOCKET3_SETTINGS      86
#define I2C_GET_SOCKET4_SETTINGS      87
#define I2C_GET_SOCKET5_SETTINGS      88  

#define I2C_SET_SENSOR1_ENABLE        24
#define I2C_SET_SENSOR2_ENABLE        25
#define I2C_SET_SENSOR3_ENABLE        26
#define I2C_SET_SENSOR4_ENABLE        27
#define I2C_SET_SENSOR5_ENABLE        28

#define I2C_ALARM_RESET               94

#define I2C_SET_SOCKET1_ON            99
#define I2C_SET_SOCKET1_OFF          104
#define I2C_SET_SOCKET1_RESET        109
#define I2C_SET_SOCKET2_ON           100
#define I2C_SET_SOCKET2_OFF          105
#define I2C_SET_SOCKET2_RESET        110
#define I2C_SET_SOCKET3_ON           101
#define I2C_SET_SOCKET3_OFF          106
#define I2C_SET_SOCKET3_RESET        111
#define I2C_SET_SOCKET4_ON           102
#define I2C_SET_SOCKET4_OFF          107
#define I2C_SET_SOCKET4_RESET        112
#define I2C_SET_SOCKET5_ON           103
#define I2C_SET_SOCKET5_OFF          108
#define I2C_SET_SOCKET5_RESET        113 //*



#define I2C_GET_FEEDPAUSE              7
#define I2C_SET_FEEDPAUSE              8



#define I2C_GET_SPECIAL_INSTRUCTION  255

/** 
 * Speciale instructies (na I2C commando 255)
 */
#define SPECIAL_INSTRUCTION_HARD_RESET              112

bool getI2CResponse(Sensor sensors[5], Socket sockets[5], byte bytesFromWiFiModule[17], bool &doReloadSettings, byte dataToWiFiModule[16], byte &specialCommandToWiFiModule, bool &feedPause);
void setSensorSettings(Sensor sensors[5], unsigned int sensorIndex, byte dataFromWiFiModule[16]);

#endif
