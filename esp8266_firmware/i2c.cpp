/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */

#include "i2c.h"

// memorytools eeprom object
MemoryTools MemTools;

/**
 * Checks data integrity. If all bytes are 255, something went wrong
 * on the i2c bus which can be caused by multiple things.
 * 
 * @param           data array to check
 *
 * @return          true if ok, otherwise false
 */
bool I2C::checkDataIntegrity(byte data[16]) {
  
   // initialize a boolean to check if not all bytes have value 255.
   // if all bytes are 255 there may be interference on the i2c bus
   // or the connection with Pond[CTRL]™ is (temporarily) lost. 
   // for (to me) unknown reasons, this happens every now and then.
   // when this happens, the same request is processed once again.
   bool allFailed = true;

   // iterate through all received bytes
   for (unsigned int byteIterator = 0; byteIterator < 16; byteIterator ++) {

        // current byte 255?
        if (data[byteIterator] == 255) {
            
            // yes, still all failed
            allFailed &= true;

        // current byte not 255?
        } else {

            // nope, means we received proper data
            allFailed = false;
        }
        
   }

   // return the value of the boolean
   return !allFailed;
}

/**
 * Sends data to the i2c bus and processes the response
 * 
 * @param           command               command byte
 * @param           dataTransmitted       data to send to i2c bus
 * @param           dataReceived          data received from i2c bus
 * 
 * @return          this function does not produce a return value
 */
void I2C::processCommand(byte command, byte dataTransmitted[16], byte dataReceived[16]) {
     
      // start transmission on address 32
      Wire.beginTransmission(32); 
      
      // write command byte and data bytes to i2c bus
      Wire.write(command);
      Wire.write(dataTransmitted, 16);

      // end transmission
      Wire.endTransmission();

      // request 16 bytes from address 32
      Wire.requestFrom(32, 16);

      // iterate 16 times
      for (int i = 0; i < 16; i ++) {
        
        // read byte to data array
        dataReceived[i] = Wire.read();
      }

      // did we request a special instruction and the device send the hard reset code?
      if ((command == I2C_GET_SPECIAL_INSTRUCTIONS) && (dataReceived[0] == I2C_SPECIAL_INSTRUCTION_HARD_RESET)) {
          
          // perform a hard reset
          memTools.hardReset();   
      }
}

/**
 * Sends data to the i2c bus and processes the response. Sensor and socket data can be acquired.
 * 
 * @param           command               command byte
 * @param           dataTransmitted       data to send to i2c bus
 * @param           dataReceived          data received from i2c bus
 * @param           sensors               sensor array to fill
 * @param           sockets               socket array to fill
 * 
 * @return          this function does not produce a return value
 */
void I2C::processCommand(byte command, byte dataTransmitted[16], byte dataReceived[16], Sensor sensors[5], Socket sockets[5]) {

      // start transmission on address 32
      Wire.beginTransmission(32);  
      
      // write command byte and data bytes to i2c bus
      Wire.write(command);
      Wire.write(dataTransmitted, 16);
      
      // end transmission
      Wire.endTransmission();

      // request 16 bytes from address 32
      Wire.requestFrom(32, 16);

      // iterate 16 times
      for (int i = 0; i < 16; i ++) {
        
        // read byte to data array
        dataReceived[i] = Wire.read();
       
      }

      // sensor values requested
      //
      // actual sensor states, values and alarm for all five sensors can be acquired in one request.
      // the byte array layout is:
      //
      // [ 0] sensor enabled, bits 0 - 4:
      //     [0] sensor 1 enabled     
      //     [1] sensor 2 enabled     
      //     [2] sensor 3 enabled     
      //     [3] sensor 4 enabled     
      //     [4] sensor 5 enabled     
      // [ 1] sensor 1 value >> 8
      // [ 2] sensor 1 value & 255
      // [ 3] sensor 2 value >> 8
      // [ 4] sensor 2 value & 255
      // [ 5] sensor 3 value >> 8
      // [ 6] sensor 3 value & 255
      // [ 7] sensor 4 value >> 8
      // [ 8] sensor 4 value & 255
      // [ 9] sensor 5 value >> 8
      // [10] sensor 5 value & 255
      // [11] sensor alarm raised   --> unused since the wifi module is now controlling all alarms and calculations
      //     [0] sensor 1 alarm       
      //     [1] sensor 2 alarm       
      //     [2] sensor 3 alarm       
      //     [3] sensor 4 alarm       
      //     [4] sensor 5 alarm       
      // 
      // to calculate the sensor value use:
      //    (sensor value first byte << 8) + sensor value second byte
      //
      // to get enabled or alarm values use:
      //    (byte >> (sensorindex)) & 1;
      // please note that sensor index starts at zero
      if (command == I2C_GET_SENSOR_VALUES) {

          // iterate through all five sensors
          for (unsigned int iterator = 0; iterator < 5; iterator ++) {
            
            // calculate and store sensor value (see description above on how to)
            sensors[iterator].setValue((dataReceived[(iterator * 2) + 1] << 8) + (dataReceived[(iterator * 2) + 2]));
            
            // set sensor enabled a
            sensors[iterator].enabled = (dataReceived[0] >> iterator) & 1;
          }

          // empty alarm byte as the wifi module is now calculating alarms
          // instead of the microprocessor
          dataReceived[11] = 0;
      }

      // sensor settings requested
      // one request per sensor is required (4 unsigned ints * 2 bytes = 8 bytes per sensor)
      // yes, it could have been 5 sensors in 3 requests :-)
      //
      // the byte array layout is:
      //
      // [ 0] sensor low value >> 8   
      // [ 1] sensor low value & 255  
      // [ 2] sensor high value >> 8  
      // [ 3] sensor high value & 255  
      // [ 4] sensor low alarm >> 8 
      // [ 5] sensor low alarm & 255
      // [ 6] sensor high alarm >> 8 
      // [ 7] sensor high alarm & 255      
      // [ 8] sensor threshold >> 8 
      // [ 9] sensor threshold & 255
      //
      // to calculate values use:
      //    (value first byte << 8) + value second byte
      if ((command >= I2C_GET_SENSOR_PH1_SETTINGS) && (command <= I2C_GET_SENSOR_LEVEL_SETTINGS)) {
          
          // calculate sensor index based on memory offset
          unsigned int sensorIndex = command - I2C_GET_SENSOR_PH1_SETTINGS;

          // calculate and store sensor value (see description above on how to)
          sensors[sensorIndex].lowValue = (dataReceived[4] << 8) + dataReceived[5];
          sensors[sensorIndex].highValue = (dataReceived[6] << 8) + dataReceived[7];
          sensors[sensorIndex].lowAlarm = (dataReceived[0] << 8) + dataReceived[1];
          sensors[sensorIndex].highAlarm = (dataReceived[2] << 8) + dataReceived[3];
          sensors[sensorIndex].threshold = (dataReceived[8] << 8) + dataReceived[9];
      }



      // sensor names requested
      // one request per sensor is required (names can consist of 1-16 characters)
      //
      // the byte arrary layout is simple: character 0 = byte 0, character 1 = byte 1 ...
      //
      if ((command >= I2C_GET_SENSOR_PH1_NAME) && (command <= I2C_GET_SENSOR_LEVEL_NAME)) {

         // sensor index is calculated based on memory offset
         
         // set sensor name, converting the byte array to a string 
        sensors[(command - I2C_GET_SENSOR_PH1_NAME)].name = byteArrayToString(dataReceived, 16);
      }
}

/**
 * Converts a byte array to a string.
 * 
 * @param           byteArray          the byte array to convert
 * @param           length             array length
 * 
 * @return          returns the converted string
 */
String I2C::byteArrayToString(byte byteArray[], char length) {

  // create an empty string to store the converted value in
  String result = "";

  // iterate through the byte array
  for (char iterator = 0; iterator < length; iterator ++) {

    // append the character to the string
    result += (char) byteArray[iterator];
  }

  // return the converted string
  return result;
}
