#include "i2c.h"


void clearDataArray(char dataArray[16]) {
  for (char iterator = 0; iterator < 16; iterator ++) {
    dataArray[iterator] = 0;
  }
}

void unsignedIntToDataArray(unsigned int value, char dataArray[16]) {
  dataArray[0] = value >> 8;
  dataArray[1] = value & 255;
  for (char iterator = 2; iterator < 16; iterator ++) {
    dataArray[iterator] = 0;
  }
}

void copyByteArray(byte source[16], byte destination[16]) {
  for (char iterator = 0; iterator < 16; iterator ++) {
    destination[iterator] = source[iterator];
  }
}

void getMultipleUnsignedInts(unsigned int unsignedIntArray[], byte dataToWiFiModule[16], unsigned int arrayLength) {
  
 
  clearDataArray(dataToWiFiModule);

  for (unsigned int iterator = 0; iterator < arrayLength; iterator ++) {


    dataToWiFiModule[(iterator * 2)] = unsignedIntArray[iterator] >> 8;;
    dataToWiFiModule[(iterator * 2) + 1] = unsignedIntArray[iterator] & 255;
  }
  
}

void setSocketSettings(Socket sockets[5], unsigned int socketIndex, byte dataFromWiFiModule[16]) {

 

   // byte 0:     affectedByFeedPause
   // byte 1:  socketMode
   // bytes 2/3  maxOnTime (seconds)
   // byte 4:   current state
   sockets[socketIndex].affectedByFeedPause = dataFromWiFiModule[0];
   
   
   sockets[socketIndex].mode = dataFromWiFiModule[1];

   sockets[socketIndex].maxOnTime = (dataFromWiFiModule[2] << 8) + dataFromWiFiModule[3];

  
}

void setSensorStateChanged(Sensor sensors[5]) {
  for (unsigned int iterator = 0; iterator < 5; iterator ++) {
      sensors[iterator].stateChanged = SENSORSTATE_CHANGED;
  }
}

void setSensorCorrection(Sensor sensors[5], unsigned int sensorIndex, byte dataFromWiFiModule[16]) {
    
    int positiveCorrection = (dataFromWiFiModule[0] << 8) + dataFromWiFiModule[1];
    int negativeCorrection = (dataFromWiFiModule[2] << 8) + dataFromWiFiModule[3];

    /* Serial.print(F("+"));
     Serial.println(positiveCorrection);
     Serial.print(F("-"));
     Serial.println(negativeCorrection);

    Serial.print(F("Before:"));
    Serial.println(sensors[sensorIndex].correctionValue);
*/

    //if (positiveCorrection > 0) {
      sensors[sensorIndex].correctionValue = (sensors[sensorIndex].correctionValue + positiveCorrection);
   // } else {
      sensors[sensorIndex].correctionValue = (sensors[sensorIndex].correctionValue - negativeCorrection);
   // }

  //  Serial.print(F("After:"));
  //  Serial.println(sensors[sensorIndex].correctionValue);
    //sensors[sensorIndex].correctionValue = positiveCorrection - negativeCorrection;
}


  
void setSensorSettings(Sensor sensors[5], unsigned int sensorIndex, byte dataFromWiFiModule[16]) {

   sensors[sensorIndex].enabled = dataFromWiFiModule[0];
   sensors[sensorIndex].lowValue = (dataFromWiFiModule[1] << 8) + dataFromWiFiModule[2];
   sensors[sensorIndex].highValue = (dataFromWiFiModule[3] << 8) + dataFromWiFiModule[4];
   sensors[sensorIndex].threshold = (dataFromWiFiModule[5] << 8) + dataFromWiFiModule[6];
   sensors[sensorIndex].alarmLow = (dataFromWiFiModule[7] << 8) + dataFromWiFiModule[8];   
   sensors[sensorIndex].alarmHigh = (dataFromWiFiModule[9] << 8) + dataFromWiFiModule[10];
}

// deprecated
bool checkAlarmRaised(Sensor sensors[5], Socket sockets[5]) {
    bool isRaised = false;

    for (unsigned int iterator = 0; iterator < 5; iterator ++) {
       isRaised |= sensors[iterator].alarmRaised;
       isRaised |= sockets[iterator].alarmRaised;
    }

    return isRaised;
}

void overrideSocket(Socket &socket, char mode) {
      socket.override = true;
      digitalWrite(socket.pinNumber, mode);
}

void resetSocket(Socket &socket) {
    socket.override = false;
    
}

bool getI2CResponse(Sensor sensors[5], Socket sockets[5], byte bytesFromWiFiModule[17], bool &doReloadSettings, byte dataToWiFiModule[16], byte &specialCommandToWiFiModule, bool &feedPause) {


  byte requestFromESP = bytesFromWiFiModule[0];


  //clearDataArray(dataToWiFiModule);

  byte dataFromWiFiModule[16];

  for (unsigned int iterator = 0; iterator < 16; iterator ++) {
    dataFromWiFiModule[iterator] = bytesFromWiFiModule[iterator + 1];
  }


  // switch statement for request, received from ESP module
  switch (requestFromESP) {

    // get PondCTRL device ID - WiFi module checks if response is 1984 (refers to my manufacturing year)
    // to identify as PondCTRL
    case I2C_GET_DEVICE_ID: { unsignedIntToDataArray(1984, dataToWiFiModule); } break;

    // ESP requests if a special instruction has been given. For example: instruction 112 tells the module
    // to perform a hard reset. See i2c.h for all special instructions
    case I2C_GET_SPECIAL_INSTRUCTION: { clearDataArray(dataToWiFiModule); dataToWiFiModule[0] = specialCommandToWiFiModule; specialCommandToWiFiModule = 0; } break;
    
    // get sensor values
    case I2C_GET_SENSOR_PH1_VALUE: {  unsignedIntToDataArray(sensors[0].value, dataToWiFiModule); } break;
    case I2C_GET_SENSOR_PH2_VALUE: {  unsignedIntToDataArray(sensors[1].value, dataToWiFiModule); } break;
    case I2C_GET_SENSOR_TEMP1_VALUE: {  unsignedIntToDataArray(sensors[2].value, dataToWiFiModule); } break;
    case I2C_GET_SENSOR_TEMP2_VALUE: {  unsignedIntToDataArray(sensors[3].value, dataToWiFiModule); } break;
    case I2C_GET_SENSOR_LEVEL_VALUE: {  unsignedIntToDataArray(sensors[4].value, dataToWiFiModule); } break;
    case I2C_GET_SENSOR_VALUES: {

      // clear data array
      // generates an array with 5*3 bytes:
      // each set of 3 bytes consist of:
      // byte 1: enabled (1 = yes, 0 = no)
      // byte 2-3: value

      
    /* for (unsigned int iterator = 0; iterator < 5; iterator ++) {
        Serial.print(iterator);
        Serial.print(" ");
        Serial.println(sensors[iterator].value);
     }*/

      
      dataToWiFiModule[0] = (sensors[4].enabled << 4) + (sensors[3].enabled << 3) + (sensors[2].enabled << 2) + (sensors[1].enabled << 1) + sensors[0].enabled;
      dataToWiFiModule[1] = sensors[0].value >> 8;
      dataToWiFiModule[2] = sensors[0].value & 255;
      dataToWiFiModule[3] = sensors[1].value >> 8;
      dataToWiFiModule[4] = sensors[1].value & 255;
      dataToWiFiModule[5] = sensors[2].value >> 8;
      dataToWiFiModule[6] = sensors[2].value & 255;
      dataToWiFiModule[7] = sensors[3].value >> 8;
      dataToWiFiModule[8] = sensors[3].value & 255;
      dataToWiFiModule[9] = sensors[4].value >> 8;
      dataToWiFiModule[10] = sensors[4].value & 255;
      dataToWiFiModule[11] = 0; // (sensors[4].alarmRaised << 4) + (sensors[3].alarmRaised << 3) + (sensors[2].alarmRaised << 2) + (sensors[1].alarmRaised << 1) + sensors[0].alarmRaised;
      dataToWiFiModule[12] = 0; // checkAlarmRaised(sensors, sockets);
      dataToWiFiModule[13] = 0; // feedPause;
   } break;

// alarm sound and autoreset are now handled on the wifi module
//  case I2C_GET_ALARM_SOUND: { dataToWiFiModule[0] = !alarmMute; } break;
//  case I2C_SET_ALARM_SOUND: { alarmMute = dataFromWiFiModule[0]; return true; } break;
//  case I2C_GET_ALARM_AUTORESET: { dataToWiFiModule[0] = alarmAutoReset; } break;
//  case I2C_SET_ALARM_AUTORESET: { alarmAutoReset = dataFromWiFiModule[0]; return true; } break;
  case I2C_GET_FEEDPAUSE: { dataToWiFiModule[0] = feedPause; } break;
  case I2C_SET_FEEDPAUSE: { setSensorStateChanged(sensors); feedPause = dataFromWiFiModule[0]; } break;

  case I2C_SET_ALARM_ON: { digitalWrite(PIN_SPEAKER, HIGH); } break;
  case I2C_SET_ALARM_OFF: { digitalWrite(PIN_SPEAKER, LOW); } break;
  case I2C_SET_LED_ON: { digitalWrite(PIN_LED, HIGH); } break;
  case I2C_SET_LED_OFF: { digitalWrite(PIN_LED, LOW); } break;
  
  // alarmAutoReset

  case I2C_ALARM_RESET: {
    for (unsigned int iterator = 0; iterator < 5; iterator ++) {
      sensors[iterator].alarmRaised = false;
      sockets[iterator].alarmRaised = false;
    }
  } break;
  
  case I2C_GET_SENSOR_PH1_SETTINGS: { unsigned int values[5] =  {sensors[0].alarmLow, sensors[0].alarmHigh, sensors[0].lowValue, sensors[0].highValue, sensors[0].threshold}; getMultipleUnsignedInts(values, dataToWiFiModule, 5); } break;
  case I2C_GET_SENSOR_PH2_SETTINGS: { unsigned int values[5] =  {sensors[1].alarmLow, sensors[1].alarmHigh, sensors[1].lowValue, sensors[1].highValue, sensors[1].threshold}; getMultipleUnsignedInts(values, dataToWiFiModule, 5); } break;
  case I2C_GET_SENSOR_TEMP1_SETTINGS: { unsigned int values[5] =  {sensors[2].alarmLow, sensors[2].alarmHigh, sensors[2].lowValue, sensors[2].highValue, sensors[2].threshold}; getMultipleUnsignedInts(values, dataToWiFiModule, 5); } break;
  case I2C_GET_SENSOR_TEMP2_SETTINGS: { unsigned int values[5] =  {sensors[3].alarmLow, sensors[3].alarmHigh, sensors[3].lowValue, sensors[3].highValue, sensors[3].threshold}; getMultipleUnsignedInts(values, dataToWiFiModule, 5); } break;
  case I2C_GET_SENSOR_LEVEL_SETTINGS: { unsigned int values[5] = {sensors[4].alarmLow, sensors[4].alarmHigh, sensors[4].lowValue, sensors[4].highValue, sensors[4].threshold}; getMultipleUnsignedInts(values, dataToWiFiModule, 5); } break;

   case I2C_SET_PH1_NAME: { copyByteArray(dataFromWiFiModule, sensors[0].name); } break;
   case I2C_SET_PH2_NAME: { copyByteArray(dataFromWiFiModule, sensors[1].name); } break;
   case I2C_SET_TEMP1_NAME: { copyByteArray(dataFromWiFiModule, sensors[2].name); } break;
   case I2C_SET_TEMP2_NAME: { copyByteArray(dataFromWiFiModule, sensors[3].name); } break;
   case I2C_SET_LEVEL_NAME: { copyByteArray(dataFromWiFiModule, sensors[4].name); } break;

    case I2C_GET_SENSOR_PH1_NAME: { copyByteArray(sensors[0].name, dataToWiFiModule); } break;
    case I2C_GET_SENSOR_PH2_NAME: { copyByteArray(sensors[1].name, dataToWiFiModule); } break;
    case I2C_GET_SENSOR_TEMP1_NAME: { copyByteArray(sensors[2].name, dataToWiFiModule); } break;
    case I2C_GET_SENSOR_TEMP2_NAME: { copyByteArray(sensors[3].name, dataToWiFiModule); } break;
    case I2C_GET_SENSOR_LEVEL_NAME: { copyByteArray(sensors[4].name, dataToWiFiModule); } break;
  
    case I2C_SET_TEMP1_CORRECTION: { setSensorCorrection(sensors, 2, dataFromWiFiModule); return true; } break;
    case I2C_SET_TEMP2_CORRECTION: { setSensorCorrection(sensors, 3, dataFromWiFiModule); return true; } break;

    case I2C_SET_PH1_CALIBRATION4: { sensors[0].adcPH4 = analogRead(PIN_PH1); return true; } break;
    case I2C_SET_PH1_CALIBRATION7: { sensors[0].adcPH7 = analogRead(PIN_PH1); return true; } break;
    case I2C_SET_PH1_CALIBRATIONRESET: { sensors[0].adcPH4 = 285; sensors[0].adcPH7 = 475; return true; } break;

    case I2C_SET_PH2_CALIBRATION4: { sensors[1].adcPH4 = analogRead(PIN_PH2); return true; } break;
    case I2C_SET_PH2_CALIBRATION7: { sensors[1].adcPH7 = analogRead(PIN_PH2); return true; } break;
    case I2C_SET_PH2_CALIBRATIONRESET: { sensors[1].adcPH4 = 285; sensors[1].adcPH7 = 475; return true; } break;

    case I2C_SET_PH1_VALUES: { setSensorSettings(sensors, 0, dataFromWiFiModule); return true; } break;
    case I2C_SET_PH2_VALUES: { setSensorSettings(sensors, 1, dataFromWiFiModule); return true; } break;
    case I2C_SET_TEMP1_VALUES: { setSensorSettings(sensors, 2, dataFromWiFiModule); return true; } break;
    case I2C_SET_TEMP2_VALUES: { setSensorSettings(sensors, 3, dataFromWiFiModule); return true; } break;
    case I2C_SET_LEVEL_VALUES: { setSensorSettings(sensors, 4, dataFromWiFiModule); return true; }    break;

    case I2C_SET_SOCKET1_NAME: { copyByteArray(dataFromWiFiModule, sockets[0].name); return true; } break; 
    case I2C_SET_SOCKET2_NAME: { copyByteArray(dataFromWiFiModule, sockets[1].name); return true; } break; 
    case I2C_SET_SOCKET3_NAME: { copyByteArray(dataFromWiFiModule, sockets[2].name); return true; } break; 
    case I2C_SET_SOCKET4_NAME: { copyByteArray(dataFromWiFiModule, sockets[3].name); return true; } break; 
    case I2C_SET_SOCKET5_NAME: { copyByteArray(dataFromWiFiModule, sockets[4].name); return true; } break; 
    
    case I2C_SET_SOCKET1_VALUES: { setSocketSettings(sockets, 0, dataFromWiFiModule); return true; } break;
    case I2C_SET_SOCKET2_VALUES: { setSocketSettings(sockets, 1, dataFromWiFiModule); return true; } break;
    case I2C_SET_SOCKET3_VALUES: { setSocketSettings(sockets, 2, dataFromWiFiModule); return true; } break;
    case I2C_SET_SOCKET4_VALUES: { setSocketSettings(sockets, 3, dataFromWiFiModule); return true; } break;
    case I2C_SET_SOCKET5_VALUES: { setSocketSettings(sockets, 4, dataFromWiFiModule); return true; } break;
    
    case I2C_GET_SOCKET1_NAME: { copyByteArray(sockets[0].name, dataToWiFiModule); } break;
    case I2C_GET_SOCKET2_NAME: { copyByteArray(sockets[1].name, dataToWiFiModule); } break;
    case I2C_GET_SOCKET3_NAME: { copyByteArray(sockets[2].name, dataToWiFiModule); } break;
    case I2C_GET_SOCKET4_NAME: { copyByteArray(sockets[3].name, dataToWiFiModule); } break;
    case I2C_GET_SOCKET5_NAME: { copyByteArray(sockets[4].name, dataToWiFiModule); } break;

    case I2C_GET_SOCKET1_SETTINGS: { dataToWiFiModule[0] = sockets[0].affectedByFeedPause; dataToWiFiModule[1] = sockets[0].mode; dataToWiFiModule[2] = sockets[0].maxOnTime << 8; dataToWiFiModule[3] = sockets[0].maxOnTime & 255; dataToWiFiModule[4] = digitalRead(sockets[0].pinNumber); dataToWiFiModule[5] = sockets[0].alarmRaised; dataToWiFiModule[6] = checkAlarmRaised(sensors, sockets); dataToWiFiModule[7] = feedPause; } break;
    case I2C_GET_SOCKET2_SETTINGS: { dataToWiFiModule[0] = sockets[1].affectedByFeedPause; dataToWiFiModule[1] = sockets[1].mode; dataToWiFiModule[2] = sockets[1].maxOnTime << 8; dataToWiFiModule[3] = sockets[1].maxOnTime & 255; dataToWiFiModule[4] = digitalRead(sockets[1].pinNumber); dataToWiFiModule[5] = sockets[1].alarmRaised; dataToWiFiModule[6] = checkAlarmRaised(sensors, sockets); dataToWiFiModule[7] = feedPause; } break;
    case I2C_GET_SOCKET3_SETTINGS: { dataToWiFiModule[0] = sockets[2].affectedByFeedPause; dataToWiFiModule[1] = sockets[2].mode; dataToWiFiModule[2] = sockets[2].maxOnTime << 8; dataToWiFiModule[3] = sockets[2].maxOnTime & 255; dataToWiFiModule[4] = digitalRead(sockets[2].pinNumber); dataToWiFiModule[5] = sockets[2].alarmRaised; dataToWiFiModule[6] = checkAlarmRaised(sensors, sockets); dataToWiFiModule[7] = feedPause; } break;
    case I2C_GET_SOCKET4_SETTINGS: { dataToWiFiModule[0] = sockets[3].affectedByFeedPause; dataToWiFiModule[1] = sockets[3].mode; dataToWiFiModule[2] = sockets[3].maxOnTime << 8; dataToWiFiModule[3] = sockets[3].maxOnTime & 255; dataToWiFiModule[4] = digitalRead(sockets[3].pinNumber); dataToWiFiModule[5] = sockets[3].alarmRaised; dataToWiFiModule[6] = checkAlarmRaised(sensors, sockets); dataToWiFiModule[7] = feedPause; } break;
    case I2C_GET_SOCKET5_SETTINGS: { dataToWiFiModule[0] = sockets[4].affectedByFeedPause; dataToWiFiModule[1] = sockets[4].mode; dataToWiFiModule[2] = sockets[4].maxOnTime << 8; dataToWiFiModule[3] = sockets[4].maxOnTime & 255; dataToWiFiModule[4] = digitalRead(sockets[4].pinNumber); dataToWiFiModule[5] = sockets[4].alarmRaised; dataToWiFiModule[6] = checkAlarmRaised(sensors, sockets); dataToWiFiModule[7] = feedPause; } break;

    case I2C_SET_SOCKET1_ON: { overrideSocket(sockets[0], HIGH); } break;
    case I2C_SET_SOCKET1_OFF: { overrideSocket(sockets[0], LOW); } break;
    case I2C_SET_SOCKET1_RESET: { resetSocket(sockets[0]); setSensorStateChanged(sensors); } break;
    case I2C_SET_SOCKET2_ON: { overrideSocket(sockets[1], HIGH); } break;
    case I2C_SET_SOCKET2_OFF: { overrideSocket(sockets[1], LOW); } break;
    case I2C_SET_SOCKET2_RESET: { resetSocket(sockets[1]); setSensorStateChanged(sensors); } break;
    case I2C_SET_SOCKET3_ON: { overrideSocket(sockets[2], HIGH); } break;
    case I2C_SET_SOCKET3_OFF: { overrideSocket(sockets[2], LOW); } break;
    case I2C_SET_SOCKET3_RESET: { resetSocket(sockets[2]); setSensorStateChanged(sensors); } break;
    case I2C_SET_SOCKET4_ON: { overrideSocket(sockets[3], HIGH); } break;
    case I2C_SET_SOCKET4_OFF: { overrideSocket(sockets[3], LOW); } break;
    case I2C_SET_SOCKET4_RESET: { resetSocket(sockets[3]); setSensorStateChanged(sensors); } break;
    case I2C_SET_SOCKET5_ON: { overrideSocket(sockets[4], HIGH); } break;
    case I2C_SET_SOCKET5_OFF: { overrideSocket(sockets[4], LOW); } break;
    case I2C_SET_SOCKET5_RESET: { resetSocket(sockets[4]); setSensorStateChanged(sensors); } break;

    /**
     * #define I2C_SET_SOCKET1_ON            99
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
#define I2C_SET_SOCKET5_RESET        11
     * 
     * 
     */
  }


  return false;
}
