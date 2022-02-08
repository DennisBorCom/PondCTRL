#include "memory.h"
#include "arduino.h"
#include "EEPROM.h"
#include "convert.h"
#include "settings.h"

char readByteFromMemory(int memoryOffset) {
  return EEPROM.read(memoryOffset);
}


unsigned int readUnsignedIntFromMemory(int memoryOffset) {
  
  return (EEPROM.read(memoryOffset) << 8) + EEPROM.read(memoryOffset + 1);
  
  /*byte byteArray[2];
  byteArray[0] = ;
  byteArray[1] = 
  unsigned int returnValue;
  charArrayToUnsignedInt(byteArray, returnValue);
  return returnValue;*/
}

void writeBytesToMemory(int memoryOffset, byte bytes[], unsigned int byteLength) {

  for (unsigned int memoryIterator = 0; memoryIterator < byteLength; memoryIterator ++) {
    EEPROM.write(memoryOffset + memoryIterator, bytes[memoryIterator]);
  }
  
}

void readBytesFromMemory(int memoryOffset, byte bytes[], unsigned int byteLength) {
   for (unsigned int memoryIterator = 0; memoryIterator < byteLength; memoryIterator ++) {
    bytes[memoryIterator] = EEPROM.read(memoryOffset + memoryIterator);
  }
}



void writeByteToMemory(int memoryOffset, char value) {
  EEPROM.write(memoryOffset, value);
}

void writeUnsignedIntToMemory(int memoryOffset, int value) {

  byte byteArray[2];

  unsignedIntToCharArray(value, byteArray);
  
  EEPROM.write(memoryOffset, byteArray[0]);
  EEPROM.write(memoryOffset + 1, byteArray[1]);
}

/**
 * Resets all initial settings
 */
void hardReset() {

  // serial debug information
  //Serial.println(F("[system          ] Performing hard reset."));

  // enable/disable all sensors
  writeByteToMemory(EEPROM_SENSOR1_ENABLED, DEFAULT_SENSOR1_ENABLED);      
  writeByteToMemory(EEPROM_SENSOR2_ENABLED, DEFAULT_SENSOR2_ENABLED);      
  writeByteToMemory(EEPROM_SENSOR3_ENABLED, DEFAULT_SENSOR3_ENABLED);     
  writeByteToMemory(EEPROM_SENSOR4_ENABLED, DEFAULT_SENSOR4_ENABLED);      
  writeByteToMemory(EEPROM_SENSOR5_ENABLED, DEFAULT_SENSOR5_ENABLED);      


  // reset pH sensor 1 default adc reference, low, high and threshold values
  writeUnsignedIntToMemory(EEPROM_PH1_4_CALIBRATION, 285);
  writeUnsignedIntToMemory(EEPROM_PH1_7_CALIBRATION, 475);
  writeUnsignedIntToMemory(EEPROM_PH1_LOW, DEFAULT_PH1_LOW);
  writeUnsignedIntToMemory(EEPROM_PH1_HIGH, DEFAULT_PH1_HIGH);
  writeUnsignedIntToMemory(EEPROM_PH1_THRESHOLD, DEFAULT_PH1_THRESHOLD);
  writeBytesToMemory(EEPROM_PH1_NAME, new byte[16] DEFAULT_PH1_NAME, 16);
  writeUnsignedIntToMemory(EEPROM_PH1_ALARM_LOW, DEFAULT_PH1_ALARM_LOW);
  writeUnsignedIntToMemory(EEPROM_PH1_ALARM_HIGH, DEFAULT_PH1_ALARM_HIGH);
  
  // reset pH sensor 2 default adc reference, low, high and threshold values
  writeUnsignedIntToMemory(EEPROM_PH2_4_CALIBRATION, 285);
  writeUnsignedIntToMemory(EEPROM_PH2_7_CALIBRATION, 475);
  writeUnsignedIntToMemory(EEPROM_PH2_LOW, DEFAULT_PH2_LOW);
  writeUnsignedIntToMemory(EEPROM_PH2_HIGH, DEFAULT_PH2_HIGH);
  writeUnsignedIntToMemory(EEPROM_PH2_THRESHOLD, DEFAULT_PH2_THRESHOLD);
  writeBytesToMemory(EEPROM_PH2_NAME, new byte[16] DEFAULT_PH2_NAME, 16);
  writeUnsignedIntToMemory(EEPROM_PH2_ALARM_LOW, DEFAULT_PH2_ALARM_LOW);
  writeUnsignedIntToMemory(EEPROM_PH2_ALARM_HIGH, DEFAULT_PH2_ALARM_HIGH);
  

  // reset temp sensor 1 default low, high and threshold values
  writeUnsignedIntToMemory(EEPROM_TEMP1_LOW, DEFAULT_TEMP1_LOW);
  writeUnsignedIntToMemory(EEPROM_TEMP1_HIGH, DEFAULT_TEMP1_HIGH);
  writeUnsignedIntToMemory(EEPROM_TEMP1_THRESHOLD, DEFAULT_TEMP1_THRESHOLD);
  writeBytesToMemory(EEPROM_TEMP1_NAME, new byte[16] DEFAULT_TEMP1_NAME, 16);
  writeUnsignedIntToMemory(EEPROM_TEMP1_ALARM_LOW, DEFAULT_TEMP1_ALARM_LOW);
  writeUnsignedIntToMemory(EEPROM_TEMP1_ALARM_HIGH, DEFAULT_TEMP1_ALARM_HIGH);
  

  // reset temp sensor 2 default low, high and threshold values
  writeUnsignedIntToMemory(EEPROM_TEMP2_LOW, DEFAULT_TEMP2_LOW);
  writeUnsignedIntToMemory(EEPROM_TEMP2_HIGH, DEFAULT_TEMP2_HIGH);
  writeUnsignedIntToMemory(EEPROM_TEMP2_THRESHOLD, DEFAULT_TEMP2_THRESHOLD);
  writeBytesToMemory(EEPROM_TEMP2_NAME, new byte[16] DEFAULT_TEMP2_NAME, 16);
  writeUnsignedIntToMemory(EEPROM_TEMP2_ALARM_LOW, DEFAULT_TEMP2_ALARM_LOW);
  writeUnsignedIntToMemory(EEPROM_TEMP2_ALARM_HIGH, DEFAULT_TEMP2_ALARM_HIGH);
  

  // set level switch name
  writeBytesToMemory(EEPROM_LEVEL_NAME, new byte[16] DEFAULT_LEVEL_NAME, 16);
  writeUnsignedIntToMemory(EEPROM_LEVEL_ALARM_LOW, DEFAULT_LEVEL_ALARM_LOW);
  writeUnsignedIntToMemory(EEPROM_LEVEL_ALARM_HIGH, DEFAULT_LEVEL_ALARM_HIGH);
  

  // reset socket 1 defaults
  writeByteToMemory(EEPROM_SOCKET1_OPERATION, DEFAULT_SOCKET1_MODE);
  writeByteToMemory(EEPROM_SOCKET1_AFFECTEDBYFEEDPAUSE, DEFAULT_SOCKET1_AFFECTEDBYFEEDPAUSE);
  writeUnsignedIntToMemory(EEPROM_SOCKET1_MAXONTIME, DEFAULT_SOCKET1_MAXONTIME);
  writeBytesToMemory(EEPROM_SOCKET1_NAME, new byte[16] DEFAULT_SOCKET1_NAME, 16);

  // reset socket 2 defaults
  writeByteToMemory(EEPROM_SOCKET2_OPERATION, DEFAULT_SOCKET2_MODE);
  writeByteToMemory(EEPROM_SOCKET2_AFFECTEDBYFEEDPAUSE, DEFAULT_SOCKET2_AFFECTEDBYFEEDPAUSE);
  writeUnsignedIntToMemory(EEPROM_SOCKET2_MAXONTIME, DEFAULT_SOCKET2_MAXONTIME);
  writeBytesToMemory(EEPROM_SOCKET2_NAME, new byte[16] DEFAULT_SOCKET2_NAME, 16);
  
  // reset socket 3 defaults
  writeByteToMemory(EEPROM_SOCKET3_OPERATION, DEFAULT_SOCKET3_MODE);
  writeByteToMemory(EEPROM_SOCKET3_AFFECTEDBYFEEDPAUSE, DEFAULT_SOCKET3_AFFECTEDBYFEEDPAUSE);
  writeUnsignedIntToMemory(EEPROM_SOCKET3_MAXONTIME, DEFAULT_SOCKET3_MAXONTIME);
  writeBytesToMemory(EEPROM_SOCKET3_NAME, new byte[16] DEFAULT_SOCKET3_NAME, 16);

  // reset socket 4 defaults
  writeByteToMemory(EEPROM_SOCKET4_OPERATION, DEFAULT_SOCKET4_MODE);
  writeByteToMemory(EEPROM_SOCKET4_AFFECTEDBYFEEDPAUSE, DEFAULT_SOCKET4_AFFECTEDBYFEEDPAUSE);
  writeUnsignedIntToMemory(EEPROM_SOCKET4_MAXONTIME, DEFAULT_SOCKET4_MAXONTIME);
  writeBytesToMemory(EEPROM_SOCKET4_NAME, new byte[16] DEFAULT_SOCKET4_NAME, 16);

  // reset socket 5 defaults
  writeByteToMemory(EEPROM_SOCKET5_OPERATION, DEFAULT_SOCKET5_MODE);  
  writeByteToMemory(EEPROM_SOCKET5_AFFECTEDBYFEEDPAUSE, DEFAULT_SOCKET5_AFFECTEDBYFEEDPAUSE);
  writeUnsignedIntToMemory(EEPROM_SOCKET5_MAXONTIME, DEFAULT_SOCKET5_MAXONTIME);
  writeBytesToMemory(EEPROM_SOCKET5_NAME, new byte[16] DEFAULT_SOCKET5_NAME, 16);

  writeUnsignedIntToMemory(EEPROM_TEMP1_CALIBRATION_POSITIVE, 0);
  writeUnsignedIntToMemory(EEPROM_TEMP1_CALIBRATION_NEGATIVE, 0);
  writeUnsignedIntToMemory(EEPROM_TEMP2_CALIBRATION_POSITIVE, 0);
  writeUnsignedIntToMemory(EEPROM_TEMP2_CALIBRATION_NEGATIVE, 0);
}

void updateEEPROM(Sensor sensors[5], Socket sockets[5]) {

 // Serial.println(F("[System] Storing settings..."));

  

 
  //  writeByteToMemory(EEPROM_ALARM_MUTE, alarmMute);
  // writeByteToMemory(EEPROM_ALARM_AUTORESET, alarmAutoReset);
   
 //Serial.println(alarmMute);
  //Serial.println((int)readByteFromMemory(EEPROM_ALARM_MUTE));

  /**
   * 
   * Sensor settings are stored in 26 byte blocks, stored after each other, so we can work with offsets and iterators.
   * 
   * byte 0+1:    value low
   * byte 2+3:    value high
   * byte 5+6:    threshold
   * byte 7+8:    alarm low
   * byte 9+10:   alarm high
   * byte 11-26:  name
   */
   
  for (unsigned int iterator = 0; iterator < 5; iterator ++) {

        writeByteToMemory((EEPROM_SENSOR1_ENABLED + iterator), sensors[iterator].enabled);
       
 
        writeBytesToMemory((EEPROM_PH1_NAME + (iterator * 26)), sensors[iterator].name, 16);      
         writeUnsignedIntToMemory((EEPROM_PH1_LOW + (iterator * 26)), sensors[iterator].lowValue);
        writeUnsignedIntToMemory((EEPROM_PH1_HIGH + (iterator * 26)), sensors[iterator].highValue);
        writeUnsignedIntToMemory((EEPROM_PH1_THRESHOLD + (iterator * 26)), sensors[iterator].threshold);
       writeUnsignedIntToMemory((EEPROM_PH1_ALARM_LOW + (iterator * 26)), sensors[iterator].alarmLow);
      writeUnsignedIntToMemory((EEPROM_PH1_ALARM_HIGH + (iterator * 26)), sensors[iterator].alarmHigh);

       
        // ph sensor; store adc calibration data
        if ((iterator == 0) || (iterator == 1)) {
          writeUnsignedIntToMemory((EEPROM_PH1_4_CALIBRATION + (iterator * 4)), sensors[iterator].adcPH4);
          writeUnsignedIntToMemory((EEPROM_PH1_7_CALIBRATION + (iterator * 4)), sensors[iterator].adcPH7);        
        }

        
        // temp sensor; store calibration data
        if ((iterator == 2) || (iterator == 3)) {

            if (sensors[iterator].correctionValue > 0) {

            //  Serial.println("Storing positive value");

              writeUnsignedIntToMemory((EEPROM_TEMP1_CALIBRATION_POSITIVE + ((iterator - 2) * 4)), sensors[iterator].correctionValue);
              writeUnsignedIntToMemory((EEPROM_TEMP1_CALIBRATION_NEGATIVE + ((iterator - 2) * 4)), 0);

          //    Serial.print(F("+"));
           //   Serial.println((int) readUnsignedIntFromMemory(EEPROM_TEMP1_CALIBRATION_POSITIVE + ((iterator - 2) * 4)));
           //   Serial.print(F("-"));
            //  Serial.println((int) readUnsignedIntFromMemory(EEPROM_TEMP1_CALIBRATION_NEGATIVE + ((iterator - 2) * 4)));
              
            } else {

/*
 * 
 * 
 *      EEPROM_TEMP1_CALIBRATION_POSITIVE      310      // 2 byte
#define EEPROM_TEMP1_CALIBRATION_NEGATIVE      312
#define EEPROM_TEMP2_CALIBRATION_POSITIVE      314
#define EEPROM_TEMP2_CALIBRATION_NEGATIVE      316 

 */

            //  Serial.println("Storing negative value");
              writeUnsignedIntToMemory((EEPROM_TEMP1_CALIBRATION_POSITIVE + ((iterator - 2) * 4)), 0);
              writeUnsignedIntToMemory((EEPROM_TEMP1_CALIBRATION_NEGATIVE + ((iterator - 2) * 4)), (0 - sensors[iterator].correctionValue));

          //    Serial.print(F("+"));
          //    Serial.println((int) readUnsignedIntFromMemory(EEPROM_TEMP1_CALIBRATION_POSITIVE + ((iterator - 2) * 4)));
          //    Serial.print(F("-"));
         //     Serial.println((int) readUnsignedIntFromMemory(EEPROM_TEMP1_CALIBRATION_NEGATIVE + ((iterator - 2) * 4)));
              
            }
        }

  
  }


  for (unsigned int iterator = 0; iterator < 5; iterator ++) {

      writeBytesToMemory((EEPROM_SOCKET1_NAME + (iterator * 16)), sockets[iterator].name, 16);
      writeByteToMemory(EEPROM_SOCKET1_AFFECTEDBYFEEDPAUSE + iterator, sockets[iterator].affectedByFeedPause);
      writeByteToMemory(EEPROM_SOCKET1_OPERATION + iterator, sockets[iterator].mode);
      writeUnsignedIntToMemory((EEPROM_SOCKET1_MAXONTIME + (iterator * 2)), sockets[iterator].maxOnTime);
      
      // statechanged: to make sure socket switching is activated
      sensors[iterator].stateChanged = SENSORSTATE_CHANGED;
  }



  
}

