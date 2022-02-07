/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */

#include "webserver.h"

// new ESP8266WebServer object at port 80 
ESP8266WebServer moduleWebServer(80); 

/**
 * Checks for active feed pause with expiired time (specified time in seconds)
 * 
 * @return          this function does not produce a return value
 */
void WebServer::checkFeedPause() {

  // current running time
  unsigned int lastLoopTime = millis() / 1000;

  // feed pause active but activation time not yet set?
  if (feedPause && (feedPauseActivated == 0)) {

    // set activation time
    feedPauseActivated = lastLoopTime;

  // feed pause active but expired (lasted longer than specified time)
  } else if (feedPause && ((lastLoopTime - feedPauseActivated) > feedpauseDuration)) {

    // iterate through all sensors
    for (unsigned int iterator = 0; iterator < 5; iterator ++) {

      // set sensor state to changed, forcing socket switching
      sensors[iterator].stateChanged = SENSORSTATE_CHANGED;
    }

    // disable feed pause
    feedPause = false;

    // set feed pause activation time to zero
    feedPauseActivated = 0;

  // feed pause active and not expired
  } else if (feedPause) {
    
   // do nothing

  // no feed pause active
  } else {

    // set last feed pause activation time to zero
    feedPauseActivated = 0;
  } 
}

/**
 * Resets the WiFi-module
 * 
 * @return          this function does not produce a return value
 */
void WebServer::reset() {
  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // boolean to hold if reset is accepted
  bool doReset = false; 
  
  // check if the user is authorized
  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // authorized
  } else {

    // set status parameter
    jsonObject.set("status", "ok");

    // set boolean to true
    doReset = true;
  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput);  

  // reset accepter?
  if (doReset) {

    // delay of one second to prevent early client disconnecting
    delay(1000);

    // yes, reset
    ESP.restart();
  }
}

/**
 * Sets calibration data for the specified temperature sensor
 * 
 * @return          this function does not produce a return value
 */
void WebServer::setTemperatureSensorCalibration() {
  
  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();
  
  // check if the user is authorized
  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // authorized
  } else {

    // parameter(s) missing?
    if (!moduleWebServer.hasArg("sensorindex") || !moduleWebServer.hasArg("correction")) {
      
      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);
    
    // invalid index? this check is required to prevent data curruption in eeprom
    // as eeprom offsets are calculated based on given socket index
    } else if ((moduleWebServer.arg("sensorindex").toInt() < 2) || (moduleWebServer.arg("sensorindex").toInt() > 3)) {
    
      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);       
      
    // request valid?     
    } else {

      // set status to ok
      jsonObject.set("status", "ok");

      // unsigned int to store command to Pond[CTRL]
      unsigned int commandByte = I2C_SET_TEMP1_CORRECTION;

      // second temperature sensor?
      if (moduleWebServer.arg("sensorindex").toInt() > 2) {
        
        // increment command byte
        commandByte ++;
      }

      int correctionValue = moduleWebServer.arg("correction").toInt();

      // byte array to hold data (and unused array from answer from I2C bus)
      byte dataToPondCTRL[16], unused[16];

      // bytes 0 + 1: positive correction value
      // bytes 2 + 3: negative correction value

      // positive correction?
      if (correctionValue > 0) {

        // set bytes
        dataToPondCTRL[0] = correctionValue >> 8;
        dataToPondCTRL[1] = correctionValue & 255;
        dataToPondCTRL[2] = 0;
        dataToPondCTRL[3] = 0;

      // negative correction?
      } else if (correctionValue < 0) {

        // convert negative to positive
        correctionValue = 0 - correctionValue;

        // set bytes
        dataToPondCTRL[0] = 0;
        dataToPondCTRL[1] = 0;
        dataToPondCTRL[2] = correctionValue >> 8;
        dataToPondCTRL[3] = correctionValue & 255;

      // no correction? why on earth would you do this?
      // supported anyway
      } else {

        // set bytes
        dataToPondCTRL[0] = 0;
        dataToPondCTRL[1] = 0;
        dataToPondCTRL[2] = 0;
        dataToPondCTRL[3] = 0;
      }

      // iterate through remaining bytes
      for (unsigned int iterator = 4; iterator < 16; iterator ++) {
        
        // set current byte to zero
        dataToPondCTRL[iterator] = 0;
      }
      
      // send i2c command to microprocessor
      i2cComm.processCommand(commandByte, dataToPondCTRL, unused);    
    }
  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput);  
}

/**
 * Sets calibration data for the specified pH sensor
 * 
 * @return          this function does not produce a return value
 */
void WebServer::setPHSensorCalibration() {
 
  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();
  
  // check if the user is authorized
  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // authorized
  } else {

    // parameter(s) missing?
    if (!moduleWebServer.hasArg("sensorindex") || !moduleWebServer.hasArg("calibrationtype")) {
      
      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);
    
    // invalid index? this check is required to prevent data curruption in eeprom
    // as eeprom offsets are calculated based on given socket index
    } else if (moduleWebServer.arg("sensorindex").toInt() > 1) {
    
      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);       
      
    // request valid?     
    } else {

      // set status to ok
      jsonObject.set("status", "ok");

      // calibration types can be:
      // 0 - Reset
      // 4 - Calibration with pH4 fluid
      // 7 - Calibration with pH7 fluid
      // invalid values will reset calibration

      // unsigned int to store command to Pond[CTRL]
      unsigned int commandByte = 0;

      // calibration type is pH4 fluid
      if (moduleWebServer.arg("calibrationtype").toInt() == 4) {

        // set command byte 
        commandByte = I2C_SET_PH1_CALIBRATION4;

      // calibration type is pH7 fluid
      } else if (moduleWebServer.arg("calibrationtype").toInt() == 7) {

        // set command byte
        commandByte = I2C_SET_PH1_CALIBRATION7;

      // calibration type other than pH4 or pH7 fluids (including invalid values)
      } else {

        // set command byte
        commandByte = I2C_SET_PH1_CALIBRATIONRESET;
      }

      // sensorindex greater than zero?
      if (moduleWebServer.arg("sensorindex").toInt() > 0) {

        // for pH4/7 fluids the offset increment is 2 for the next sensor
        // for resetting the increment is 1

        // command other than calibration reset
        if (commandByte < I2C_SET_PH1_CALIBRATIONRESET) {

          // increment the command byte with 2
          commandByte += 2;

        // command equal to reset
        } else {

          // increment the command byte with 1
          commandByte += 1;
        }
      }

      // byte array to hold data (and unused array from answer from I2C bus)
      byte dataToPondCTRL[16], unused[16];
      
      // send i2c command to microprocessor
      i2cComm.processCommand(commandByte, dataToPondCTRL, unused);    
    }

  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput); 
}

/**
 * Gets the UPnP Port number
 * 
 * @return              upnp port number
 */
unsigned int WebServer::getUPnPPort() {

    // return the port number
    return upnpPort;
}

/**
 * Sets the current socket settings
 * 
 * @return          this function does not produce a return value
 */
void WebServer::setSocket() {

  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();
  
  // check if the user is authorized

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // authorized
  } else {

    // parameter(s) missing?
    if (!moduleWebServer.hasArg("socketindex") || !moduleWebServer.hasArg("name") || !moduleWebServer.hasArg("affectedbyfeedpause") || !moduleWebServer.hasArg("mode") || !moduleWebServer.hasArg("maxontime")) {
      
      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);
    
    // invalid index? this check is required to prevent data curruption in eeprom
    // as eeprom offsets are calculated based on given socket index
    } else if (moduleWebServer.arg("socketindex").toInt() > 4) {
    
      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);       
      
    // request valid?     
    } else {

      // store name on microprocessor and set name in local variable

      // byte array to hold sensor name (and unused array from answer from I2C bus)
      byte dataToPondCTRL[16], unused[16];

      // iterate 16 times (max name length)
      for (unsigned int iterator = 0; iterator < 16; iterator ++) {

        // length larger than current iterated character?
        if (moduleWebServer.arg("name").length() > iterator) {

          // copy character to array
          dataToPondCTRL[iterator] = moduleWebServer.arg("name")[iterator];
        
        // current index larger than name length
        } else {

          // add zero to array
          dataToPondCTRL[iterator] = 0;
        }    
      }

      // calculate command byte using an offset (I2C_SET_SOCKET1_NAME + socketindex
      // the five command bytes to store names are 74, 75, 76, 77 and 78
      unsigned int commandByte = I2C_SET_SOCKET1_NAME + moduleWebServer.arg("socketindex").toInt();

      // send i2c command to microprocessor
      i2cComm.processCommand(commandByte, dataToPondCTRL, unused);

      // set name in local variables
      sockets[moduleWebServer.arg("socketindex").toInt()].name = moduleWebServer.arg("name");

      // feedback in json
      jsonObject.set("name", moduleWebServer.arg("name"));   

      // store other values on microprocessor and in local variables

      // byte 0: affected by feedpause

      // affected by feedpause set to true?
      if (moduleWebServer.arg("affectedbyfeedpause") == "true") {

        // set byte to 1
        dataToPondCTRL[0] = 1;

        // feedback in json
        jsonObject.set("affectedbyfeedpause", true);

        // set local variable
        sockets[moduleWebServer.arg("socketindex").toInt()].affectedByFeedPause = true;

      // not affected by feed pause?
      } else {

        // set byte to 0
        dataToPondCTRL[0] = 0;

        // feedback in json
        jsonObject.set("affectedbyfeedpause", false);

        // set local variable
        sockets[moduleWebServer.arg("socketindex").toInt()].affectedByFeedPause = false;
      }

      // byte 1: socket mode
      dataToPondCTRL[1] = moduleWebServer.arg("mode").toInt();

      // set local variable
      sockets[moduleWebServer.arg("socketindex").toInt()].mode = moduleWebServer.arg("mode").toInt();
      
      // feedback in json
      jsonObject.set("mode", moduleWebServer.arg("mode").toInt());

      // byte 2 + 3: max on time
      dataToPondCTRL[2] = moduleWebServer.arg("maxontime").toInt() >> 8;
      dataToPondCTRL[3] = moduleWebServer.arg("maxontime").toInt() & 255;

      // set local variable
      sockets[moduleWebServer.arg("socketindex").toInt()].maxOnTime = moduleWebServer.arg("maxontime").toInt();
      
      // feedback in json
      jsonObject.set("maxontime", moduleWebServer.arg("maxontime").toInt());

      // fill unused bytes with zeroes
      dataToPondCTRL[4] = 0;
      dataToPondCTRL[5] = 0;
      dataToPondCTRL[6] = 0;
      dataToPondCTRL[7] = 0;
      dataToPondCTRL[8] = 0;
      dataToPondCTRL[9] = 0;
      dataToPondCTRL[10] = 0;
      dataToPondCTRL[11] = 0;
      dataToPondCTRL[12] = 0;
      dataToPondCTRL[13] = 0;
      dataToPondCTRL[14] = 0;
      dataToPondCTRL[15] = 0;

      // calculate command byte using an offset (I2C_SET_SOCKET1_VALUES + socketindex
      // the five command bytes to store values are 79, 80, 81, 82 and 83
      commandByte = I2C_SET_SOCKET1_VALUES + moduleWebServer.arg("socketindex").toInt();

      // send i2c command to microprocessor
      i2cComm.processCommand(commandByte, dataToPondCTRL, unused);

      // set status to ok
      jsonObject.set("status", "ok");
    }
  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput);
}

/**
 * Sets the systemn settings
 * 
 * @return          this function does not produce a return value
 */
void WebServer::setSystem() {

  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();
 
  // check if the user is authorized

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // authorized
  } else {

    // one or more required parameters missing?
    if (!moduleWebServer.hasArg("feedpause_duration") || !moduleWebServer.hasArg("timezone_offset") || !moduleWebServer.hasArg("timeserver") || !moduleWebServer.hasArg("upnp_port")) {
      
      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);

    // all parameters available?
    } else {

      // get feedpause duration from parameters
      feedpauseDuration = moduleWebServer.arg("feedpause_duration").toInt();

      // feedpause duration is stored as a 2 byte value (value >> 8, value & 255)

      // write feed pause duration to memory
      memTools.writeCharToMemory(memTools.EEPROM_FEEDPAUSE_DURATION[0], feedpauseDuration >> 8);
      memTools.writeCharToMemory((memTools.EEPROM_FEEDPAUSE_DURATION[0] + 1), feedpauseDuration & 255);

      // get port number from parameters
      upnpPort = moduleWebServer.arg("upnp_port").toInt();
      
      // write port number to memory
      memTools.writeCharToMemory(memTools.EEPROM_UPNP_PORT[0], upnpPort >> 8);
      memTools.writeCharToMemory((memTools.EEPROM_UPNP_PORT[0] + 1), upnpPort & 255);

      // timezone offset is stored as a 4-byte value, split in 2 2-byte values.
      // byte 0 - 1 = (positive >> 8, positive & 255)
      // byte 2 - 3 = (negative >> 8, negative & 255)

      // negative offset?
      if (moduleWebServer.arg("timezone_offset").substring(0, 1) == "-") {

          // get the negative value (as positive) from parameters
         unsigned int negativeValue = moduleWebServer.arg("timezone_offset").substring(1).toInt();

         // first two bytes are empty (negative means no positive)
         memTools.writeCharToMemory(memTools.EEPROM_TIMESERVER_UTC_OFFSET[0],  0);
         memTools.writeCharToMemory((memTools.EEPROM_TIMESERVER_UTC_OFFSET[0] + 1),  0);

         // write value to memory
         memTools.writeCharToMemory((memTools.EEPROM_TIMESERVER_UTC_OFFSET[0] + 2),  negativeValue >> 8);
         memTools.writeCharToMemory((memTools.EEPROM_TIMESERVER_UTC_OFFSET[0] + 3),  negativeValue & 255);

         // set offset
         timezoneOffset = 0 - negativeValue;

      // positive offset?
      } else {

        // get the positive value
        timezoneOffset = moduleWebServer.arg("timezone_offset").toInt();

        // write value to memory
        memTools.writeCharToMemory((memTools.EEPROM_TIMESERVER_UTC_OFFSET[0]),  timezoneOffset >> 8);
        memTools.writeCharToMemory((memTools.EEPROM_TIMESERVER_UTC_OFFSET[0] + 1), timezoneOffset & 255);

        // last two bytes are empty (negative means no positive)
        memTools.writeCharToMemory((memTools.EEPROM_TIMESERVER_UTC_OFFSET[0] + 2),  0);
        memTools.writeCharToMemory((memTools.EEPROM_TIMESERVER_UTC_OFFSET[0] + 3),  0);

      }
                  
      // write time server address to memory
      memTools.writeMemory(memTools.EEPROM_TIMESERVER_ADDRESS, moduleWebServer.arg("timeserver"));

      // set time server
      timeserver = moduleWebServer.arg("timeserver");
      
      // set status to ok!
      jsonObject.set("status", "ok");
    }
  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput);
  
  // reset to update upnp settings
  reset();
}

/**
 * Sets the current sensor settings
 * 
 * @return          this function does not produce a return value
 */
void WebServer::setSensor() {

  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // check if the user is authorized

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // authorized
  } else {

    // parameter(s) missing?
    if (!moduleWebServer.hasArg("sensorindex") || !moduleWebServer.hasArg("name") || !moduleWebServer.hasArg("enabled") || !moduleWebServer.hasArg("lowvalue") || !moduleWebServer.hasArg("highvalue") || !moduleWebServer.hasArg("lowalarm") || !moduleWebServer.hasArg("highalarm") || !moduleWebServer.hasArg("threshold")) {
      
      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);

    // invalid index? this check is required to prevent data curruption in eeprom
    // as eeprom offsets are calculated based on given sensorindex
    } else if (moduleWebServer.arg("sensorindex").toInt() > 4) {

      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);       
      
    // request valid?     
    } else {

      // store name on microprocessor and set name in local variable

      // byte array to hold sensor name (and unused array from answer from I2C bus)
      byte dataToPondCTRL[16], unused[16];

      // iterate 16 times (max name length)
      for (unsigned int iterator = 0; iterator < 16; iterator ++) {

        // length larger than current iterated character?
        if (moduleWebServer.arg("name").length() > iterator) {

          // copy character to array
          dataToPondCTRL[iterator] = moduleWebServer.arg("name")[iterator];
        
        // current index larger than name length
        } else {

          // add zero to array
          dataToPondCTRL[iterator] = 0;
        }    
      }

      // calculate command byte using an offset (I2C_SET_SENSOR_PH1_NAME + (2 * sensorindex)
      // the five command bytes to store names are 64, 66, 68, 70 and 72
      unsigned int commandByte = I2C_SET_SENSOR_PH1_NAME + (moduleWebServer.arg("sensorindex").toInt() * 2);

      // send i2c command to microprocessor
      i2cComm.processCommand(commandByte, dataToPondCTRL, unused);

      // set name in local variables
      sensors[moduleWebServer.arg("sensorindex").toInt()].name = moduleWebServer.arg("name");

      // feedback in json
      jsonObject.set("name", moduleWebServer.arg("name"));

      // store other values on microprocessor and in local variables

      // byte 0: 0 = disabled, 1 = enabled
      
      // enabled set to true?
      if (moduleWebServer.arg("enabled") == "true") {

        // set byte to 1
        dataToPondCTRL[0] = 1;

        // feedback in json
        jsonObject.set("enabled", true);

        // set local variable
        sensors[moduleWebServer.arg("sensorindex").toInt()].enabled = true;

      // enabled set to false?
      } else {

        // set byte to 0
        dataToPondCTRL[0] = 0;

        // feedback in json
        jsonObject.set("enabled", false);

         // set local variable
        sensors[moduleWebServer.arg("sensorindex").toInt()].enabled = false;
      }

      // byte 1 + 2: low value
      dataToPondCTRL[1] = moduleWebServer.arg("lowvalue").toInt() >> 8;
      dataToPondCTRL[2] = moduleWebServer.arg("lowvalue").toInt() & 255;

      // set low value in local variable
      sensors[moduleWebServer.arg("sensorindex").toInt()].lowValue = moduleWebServer.arg("lowvalue").toInt();
      
      // feedback in json
      jsonObject.set("lowvalue", moduleWebServer.arg("lowvalue").toInt());

      // byte 2 + 3: high value
      dataToPondCTRL[3] = moduleWebServer.arg("highvalue").toInt() >> 8;
      dataToPondCTRL[4] = moduleWebServer.arg("highvalue").toInt() & 255;

      // set high value in local variable
      sensors[moduleWebServer.arg("sensorindex").toInt()].highValue = moduleWebServer.arg("highvalue").toInt();

      // feedback in json
      jsonObject.set("highvalue", moduleWebServer.arg("highvalue").toInt());

      // byte 5 + 6: threshold
      dataToPondCTRL[5] = moduleWebServer.arg("threshold").toInt() >> 8;
      dataToPondCTRL[6] = moduleWebServer.arg("threshold").toInt() & 255;

      // set high value in local variable
      sensors[moduleWebServer.arg("sensorindex").toInt()].threshold = moduleWebServer.arg("threshold").toInt();

      // feedback in json
      jsonObject.set("threshold", moduleWebServer.arg("threshold").toInt());

      // byte 7 + 8: low alarm
      dataToPondCTRL[7] = moduleWebServer.arg("lowalarm").toInt() >> 8;
      dataToPondCTRL[8] = moduleWebServer.arg("lowalarm").toInt() & 255;

      // set high value in local variable
      sensors[moduleWebServer.arg("sensorindex").toInt()].lowAlarm = moduleWebServer.arg("lowalarm").toInt();

      // feedback in json
      jsonObject.set("lowalarm", moduleWebServer.arg("lowalarm").toInt());

      // byte 9 + 10: high alarm
      dataToPondCTRL[9] = moduleWebServer.arg("highalarm").toInt() >> 8;
      dataToPondCTRL[10] = moduleWebServer.arg("highalarm").toInt() & 255;

      // set high value in local variable
      sensors[moduleWebServer.arg("sensorindex").toInt()].highAlarm = moduleWebServer.arg("highalarm").toInt();

      // feedback in json
      jsonObject.set("highalarm", moduleWebServer.arg("highalarm").toInt());

      // fill unused bytes with zeroes
      dataToPondCTRL[11] = 0;
      dataToPondCTRL[12] = 0;
      dataToPondCTRL[13] = 0;
      dataToPondCTRL[14] = 0;
      dataToPondCTRL[15] = 0;

      // calculate command byte using an offset (I2C_SET_SENSOR_PH1_VALUES + (2 * sensorindex)
      // the five command bytes to store values are 65, 67, 69, 71 and 73
      commandByte = I2C_SET_SENSOR_PH1_VALUES + (moduleWebServer.arg("sensorindex").toInt() * 2);

      // send i2c command to microprocessor
      i2cComm.processCommand(commandByte, dataToPondCTRL, unused);

      // set status to ok
      jsonObject.set("status", "ok");
    }
  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput);
}

/**
 * Sets the WiFi module current uptime
 * 
 * @param           timestamp       current time in seconds from last reboot
 * 
 * @return          this function does not produce a return . value
 */
void WebServer::setUptime(unsigned long timestamp) {

    // set timestamp
    uptime = timestamp;
}
/**
 * Sets custom data for debug purposes. Data is displayed in get-data
 * 
 * @param           data        custom data to rest
 * 
 * @return          this function does not produce a return value
 */
void WebServer::setCustomData(String data) {

  // set custom data
  customData = data;
}
/**
 * Sets the current sensor value
 * 
 * @return          this function does not produce a return value
 */
void WebServer::setSensorValues() {
  
  // iterate through five sensors
  for (unsigned int iterator = 0; iterator < 5; iterator ++) {
    
    // set current sensor value
    sensorValues[iterator] = sensors[iterator].getValue();
  }
}

/**
 * Set the active boolean to true, which activates alarm calculation
 * 
 * @return          this function does not produce a return value
 */
void WebServer::activateSensorData() {
  
  // set active boolean to true
  active = true;
}

/**
 * Get timezone offset
 * 
 * @return        timezone offset
 */
int WebServer::getTimezoneOffset() {

    // return the timezone offset
    return timezoneOffset;
}

/**
 * Check if the user is authorized (logged in or api key provided)
 * 
 * @return          this function does not produce a return value
 */
bool WebServer::isAuthorized() {

  // authorization is checked by comparing the client ip
  // to the logged in ip and by checking for a valid api
  // key 
  //
  // using the api key, devices can retrieve/set data 
  // without authorization

  // api key provided and stored api key not empty
  if (moduleWebServer.hasArg("apikey") && (apiKey != "")) {

    // api key correct
    if (moduleWebServer.arg("apikey") == apiKey) {

      // device authorized
      return true;
    }
  }

  // ips do not match?
  if (moduleWebServer.client().remoteIP() != administrator) {
     
     // return value false
     return false;
  }

  // return value true
  return true;
}

/**
 * Retrieves the latest firmware version from the server and updates if outdated
 * 
 * @return            latest version if not outdated otherwise module will restart without output
 */
unsigned int WebServer::getFirmwareUpdateVersion() {
  
  // create firmware version check url 
  // http://pondctrl.dennisbor.com/update/wifi/firmware.version
  String firmwareVersionURL = String(firmwareURLBase);
  firmwareVersionURL.concat(".version");

  // new httpclient object
  // currently the firmware only supports http and no https because of the required amount
  // of additional code required to verify https authentication and the limitation of 
  // 512kb firmware size without losing OTA update capability.
  HTTPClient httpClient;

  // open the generated url
  httpClient.begin(firmwareVersionURL);

  // get contents from specified url and store http status code   
  int httpCode = httpClient.GET();

  // status 200 (ok)
  if (httpCode == 200) {

    // read http response to string
    String newFirmwareVersion = httpClient.getString();

    // parse integer from version string
    unsigned int firmwareUpdateVersion = newFirmwareVersion.toInt();

    // newer firmware available?
    if (firmwareUpdateVersion > currentFirmwareVersion) {
      
      // start update
      doUpdate();
      
    } else {

      // set firmware version 
      setFirmwareUpdateVersion(firmwareUpdateVersion);
    }

  } 
  
  // end http transmission
  httpClient.end();
}

/**
 * Sets the current unix epoch timestamp
 * 
 * @param         stamp       current unix epoch timestamp
 * 
 * @return        this value does not produce a return value
 */
void WebServer::setTimestamp(unsigned long stamp) {

  // set timestamp
  timestamp = stamp;
}
/**
 * Get all information available to display in an interface
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::get() {
  
  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // check if the user is authorized

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // not checked for updates first or no update available?
  } else {

    // set parameters
    jsonObject.set("status", "ok");
    jsonObject.set("wireless_mode", module_wireless_mode);
    jsonObject.set("wireless_ssid", module_ssid);
    jsonObject.set("wireless_hostname", module_mdns_hostname);
    jsonObject.set("feedpause", feedPause);
    jsonObject.set("feedpause_duration", feedpauseDuration);
    jsonObject.set("timezone_offset", timezoneOffset);
    jsonObject.set("alarmsoundon", alarmSoundOn);
    jsonObject.set("alarmautorecovery", alarmAutoRecovery);
    jsonObject.set("username", module_gui_username);
    jsonObject.set("macaddress", WiFi.macAddress());
    jsonObject.set("active", active);
    jsonObject.set("apikey", apiKey);
    jsonObject.set("debug", customData);
    jsonObject.set("uptime", uptime);
    jsonObject.set("timestamp", timestamp);
    jsonObject.set("timestamp_without_offset", (timestamp - timezoneOffset));
    jsonObject.set("timeserver", timeserver);
    jsonObject.set("externalport", upnpPort);
    
    // feedpause activated?
    if (feedPause) {
        
        // calculate remaining duration
        jsonObject.set("feedpause_remaining", (feedpauseDuration - ((millis() / 1000) - feedPauseActivated)));
    }
     
    unsigned int timeonlystamp, seconds, minutes, hours;
    timeonlystamp = timestamp % 86400;
    seconds = timeonlystamp % 60;
    minutes = ((timeonlystamp - seconds) % 3600) / 60;
    hours = (timeonlystamp - seconds - (minutes * 60)) / 3600;

    jsonObject.set("time_seconds", seconds);
    jsonObject.set("time_minutes", minutes);
    jsonObject.set("time_hours", hours);

    // temporary garbage
/*
     $stamp = time(); //1597816659;        // 8:10 nl, 7:10 utc
    
    $hms = $stamp % 86400;
    $seconds = $hms % 60;                           // seconds is remainder of 60
    $minutes = (($hms - $seconds) % 3600 / 60);     // stamp - seconds, remainder of 3600 divided by 60
    $hours = ($hms - $seconds - ($minutes * 60)) / 3600;
    
    echo $hms.'<br/>'.$seconds.'<br/>'.$minutes.'<br/>'.$hours;

    */
    
    
    // update information
    // set currentversion parameter
    jsonObject.set("currentversion", currentFirmwareVersion);

    // set latestversion parameter
    jsonObject.set("latestversion", firmwareUpdateVersion);

    // update version higher than current version
    if (firmwareUpdateVersion > currentFirmwareVersion) {

      // set available parameter to true
      jsonObject.set("updateavailable", true);

    // update version equal to current version
    } else {

      // set available parameter to false
      jsonObject.set("updateavailable", false);
      
    }
            
    // create an array to store sensor data
    JsonArray& sensorJsonArray = jsonBuffer.createArray();

    // iterate through all sensors
    for (unsigned int iterator = 0; iterator < 5; iterator ++) {

      // create and fill a json sensor object
      JsonObject& jsonSensor = jsonBuffer.createObject();
      jsonSensor.set("name", sensors[iterator].name);
      jsonSensor.set("alarmraised", sensors[iterator].alarmRaised);
      jsonSensor.set("enabled", sensors[iterator].enabled);
      jsonSensor.set("lowvalue", sensors[iterator].lowValue);
      jsonSensor.set("highvalue", sensors[iterator].highValue);
      jsonSensor.set("lowalarm", sensors[iterator].lowAlarm);
      jsonSensor.set("highalarm", sensors[iterator].highAlarm);
      jsonSensor.set("threshold", sensors[iterator].threshold);
      jsonSensor.set("state", sensors[iterator].state);
      jsonSensor.set("currentvalue", sensors[iterator].getValue());

      // add the current ssid to the array
      sensorJsonArray.add(jsonSensor);
    }

    // add sensor array to the json object
    jsonObject.set("sensors", sensorJsonArray);

    // create an array to store socket data
    JsonArray& socketJsonArray = jsonBuffer.createArray();


    // iterate through all sockets
    for (unsigned int iterator = 0; iterator < 5; iterator ++) {

      // create and fill a json sensor object
      JsonObject& jsonSocket = jsonBuffer.createObject();
      jsonSocket.set("name", sockets[iterator].name);
      jsonSocket.set("maxontime", sockets[iterator].maxOnTime);
      jsonSocket.set("mode", sockets[iterator].mode);
      jsonSocket.set("override", sockets[iterator].override);
      jsonSocket.set("affectedbyfeedpause", sockets[iterator].affectedByFeedPause);
      jsonSocket.set("alarmraised", sockets[iterator].alarmRaised);
      jsonSocket.set("active", sockets[iterator].isActive);
      
      // add the current ssid to the array
      socketJsonArray.add(jsonSocket);
      
    }

    // add socket array to the json object
    jsonObject.set("sockets", socketJsonArray);

  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput);
}

/**
 * Get the time server address
 * 
 * @return          time server address
 */
String WebServer::getTimeserver() {

  // return time server address
  return timeserver;
}
/**
 * Toggles alarm sound either on or off
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::toggleAlarmSound() {
  
  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // check if the user is authorized

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // not checked for updates first or no update available?
  } else {

    // no 'on' argument given?
    if (!moduleWebServer.hasArg("on")) {
     
      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);

    // 'on' argument given?
    } else {

      // set status parameter to ok
      jsonObject.set("status", "ok");
      
      // on set to true?
      if (moduleWebServer.arg("on") == "true") {

        // activate alarm sound
        alarmSoundOn = true;

        // write 1 to memory (on)
        memTools.writeMemory(memTools.EEPROM_ALARM_SOUND, "1");
    
      // on set to other value than true
      } else {

        // deactivate alarm sound
        alarmSoundOn = false;

        // write 0 to memory (off)
        memTools.writeMemory(memTools.EEPROM_ALARM_SOUND, "0");
      }

      // set json parameter
      jsonObject.set("alarmsoundon", alarmSoundOn);
    }
  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput);
}

    
/**
 * Toggles alarm auto recovery either on or off
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::toggleAlarmAutoRecovery() {
   
  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // check if the user is authorized

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // not checked for updates first or no update available?
  } else {

    // no 'on' argument given?
    if (!moduleWebServer.hasArg("on")) {
     
      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);

    // 'on' argument given?
    } else {

      // set status parameter to ok
      jsonObject.set("status", "ok");
      
      // on set to true?
      if (moduleWebServer.arg("on") == "true") {

        // activate alarm sound
        alarmAutoRecovery = true;

        // write 1 to memory (on)
        memTools.writeMemory(memTools.EEPROM_ALARM_AUTORESET, "1");
    
      // on set to other value than true
      } else {

        // deactivate alarm sound
        alarmAutoRecovery = false;

        // write 0 to memory (off)
        memTools.writeMemory(memTools.EEPROM_ALARM_AUTORESET, "0");
      }

      // set json parameter
      jsonObject.set("alarmautorecovery", alarmAutoRecovery);
    }
  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput);
}
    
/**
 * Toggles feed pause either on or off
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::toggleFeedPause() {
  
  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // check if the user is authorized

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // authorized?
  } else {

    // no 'on' argument given?
    if (!moduleWebServer.hasArg("on")) {
     
      // set status parameter to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);

    // 'on' argument given?
    } else {

      // set status parameter to ok
      jsonObject.set("status", "ok");
      
      // on set to true?
      if (moduleWebServer.arg("on") == "true") {

        // activate feedpause
        feedPause = true;

      // on set to other value than true
      } else {

        // deactivate feedpause
        feedPause = false;
      }

      // set json parameter 
      jsonObject.set("feedpause", feedPause);

      // force socket switching to activate or deactivate
      // feed pause on sockets
      forceSocketSwitching();
    }
  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput);
}

/**
 * Update the WiFi-module
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::doUpdate() {

  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // firmware version not set or up to date>
  if ((firmwareUpdateVersion == 0) || (firmwareUpdateVersion == currentFirmwareVersion)) {
    
    // set status parameter to no, errno to 3 (invalid request)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 3);

  // authorized and update available
  } else {

    // set status parameter to ok
    jsonObject.set("status", "ok");
  }

  // output is sent to the client before the actual update, because
  // otherwise the connection will drop and we are unable to
  // send a response

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);

  // output json to client
  outputJSON(jsonOutput);

  // status parameter is set to no?
  if (jsonObject.get<String>("status") == "no") {
      
      // leave function
      return;
  }

  // create firmware version check url 
  // http://pondctrl.dennisbor.com/update/wifi/firmware.<VERSION>.bin
  String firmwareImageURL = String(firmwareURLBase);
  firmwareImageURL.concat(".");
  firmwareImageURL.concat(firmwareUpdateVersion);
  firmwareImageURL.concat(".bin");

  // update using specified url and store result
  t_httpUpdate_return ret = ESPhttpUpdate.update(firmwareImageURL);

  // update succeeded?
  if (ret == HTTP_UPDATE_OK) {

    // restart the WiFi module
    ESP.restart();
  }
}

/**
 * Send output to the webserver
 * 
 * @param           jsonOutput        json data in string format
 * 
 * @return          this function does not produce a return value
 */
void WebServer::outputJSON(String jsonOutput) {

  // allow cross resource sharing
  moduleWebServer.sendHeader("Access-Control-Allow-Origin", "*");
  
  // send http header and data
  moduleWebServer.send(200, "text/plain", jsonOutput);
}


/**
 * Attempts to log in to the module using the given username and password
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::doLogin() {

  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // no username or password argument received?
  if (!moduleWebServer.hasArg("username") || !moduleWebServer.hasArg("password")) {
   
    // set status parameter to no, errno to 3 (invalid request)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 3);

  // username and password correct
  } else if ((moduleWebServer.arg("username") == module_gui_username) && (moduleWebServer.arg("password") == module_gui_password)) {

    // set status parameter to ok
    jsonObject.set("status", "ok");

    // set administrator ip to client ip
    administrator = moduleWebServer.client().remoteIP();

  // username or password incorrect
  } else {

    // set status parameter to no, errno to 1 (valid login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);
    
  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput);
}

/**
 * Set credentials for loggin in to the module
 * 
 * @param                 username            module login username
 * @param                 password            module login password
 * 
 * @return          this function does not produce a return value
 */
void WebServer::setGUICredentials(String username, String password) {
    
    // set username and password to given values
    module_gui_username = username;
    module_gui_password = password;
}

/**
 * Set the device api key for interaction between devices without
 * autorization
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::setApiKey() {
  
  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // check if the user is authorized
  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // no apikey parameter received
  } else if (!moduleWebServer.hasArg("apikey")) {
    
    // set status parameter to no, errno to 3 (invalid request)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 3);

  // authorized and apikey specified
  // please note that the api key is not validated
  } else {

    // set used api key
    apiKey = moduleWebServer.arg("apikey");

    // write api key to memory
    memTools.writeMemory(memTools.EEPROM_APIKEY, moduleWebServer.arg("apikey"));
    

    // set status to ok
    jsonObject.set("status", "ok");
  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);
  
  // output json to client
  outputJSON(jsonOutput);
  
}

/**
 * Attempts to log in to the module using the given username and password
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::setWiFi() {

  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // check if the user is authorized

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // no ssid or passphrase parameters received
  } else if (!moduleWebServer.hasArg("ssid") || !moduleWebServer.hasArg("passphrase")) {
    
    // set status parameter to no, errno to 3 (invalid request)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 3);

  // authorized and ssid and passphrase given
  } else {

    // set status parameter to ok
    jsonObject.set("status", "ok");
  }

  // output is sent to the client before the actual connection attempt, 
  // because otherwise the connection will drop and we are unable to
  // send a response

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);

  // output json to client
  outputJSON(jsonOutput);

  // delay one second to prevent early disconnecting between
  // web server and client
  delay(1000);

  // save current ssid and passphrase to be able to fall back if the new
  // connection attempt fails
  String currentSSID = WiFi.SSID();
  String currentPassphrase = WiFi.psk();

  // disconnect wifi
  WiFi.disconnect();
  
  // char to register connection attempts. we will attempt a maximum of
  // 25 times with an interval of 500ms.
  char connectionAttempt = 0;

  // not yet connected and the number of attempts less than 25
  while ((WiFi.status() != WL_CONNECTED) && (connectionAttempt < 25)) {
    // attempt to connect to the specified network
    WiFi.begin(
      moduleWebServer.arg("ssid").c_str(), 
      moduleWebServer.arg("passphrase").c_str()
    );

    // delay of 500ms
    delay(500);

    // increment the connection attempt counter
    connectionAttempt ++;
  }

  // connection succesful
  if (WiFi.status() == WL_CONNECTED) {

    
    // write ssid and passphrase to memory
    memTools.writeMemory(memTools.EEPROM_SSID, moduleWebServer.arg("ssid"));
    memTools.writeMemory(memTools.EEPROM_PASSPHRASE, moduleWebServer.arg("passphrase"));

    // optional mdns hostname parameter specified?
    if (moduleWebServer.hasArg("mdns_hostname")) {

      // argument not empty
      if (moduleWebServer.arg("mdns_hostname") != "") {
      
        // write mdns responder name to memory
        memTools.writeMemory(memTools.EEPROM_MDNSRESPONDER, moduleWebServer.arg("mdns_hostname"));
  
      }
    }

    // restart WiFi module
    ESP.restart();

  // connection failed
  } else {

    // fall back to the current settings
    WiFi.begin(currentSSID.c_str(), currentPassphrase.c_str());
  }
}

/**
 * Get device mac address, used as the device serial number
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::getMac() {

  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();
 
  // check if the user is authorized

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // authorized
  } else {

    // set status to ok
    jsonObject.set("status", "ok");

    // set mac parameter
    jsonObject.set("macaddress", WiFi.macAddress());
  }
  
  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);

  // output json to client
  outputJSON(jsonOutput);
}

/**
 * Toggles the specified socket on or off, by sending an I2C command to
 * Pond[CTRL]
 * 
 * @param           socketIndex           socket number (0-4)
 * @param           toggleOn              true if on, otherwise false
 * 
 * @return          this function does not produce a return value
 */
void WebServer::toggleSocket(unsigned int socketIndex, bool toggleOn) {
  
  // calculate comnmand byte offset
  char commandByte = I2C_SET_SOCKET1_ON + socketIndex;
  
  // toggle off?
  if (!toggleOn) {

    // calculate new offset by incrementing with 5
    commandByte += 5;
  }

  // bytes array for data to and from I2C bus, both unused
  byte unused1[16], unused2[16];

  // send I2C command
  i2cComm.processCommand(commandByte, unused1, unused2);

  // set active parameter
  sockets[socketIndex].isActive = toggleOn;

  // 100ms delay to prevent power issues/microprocessor resetting
  delay(100);
}

/**
 * Calculates the socket state to set (on or off) by comparing the current mode and
 * required state
 * 
 * @param           socketIndex           index of socket to toggle
 * @param           sensorIndex           index of sensor to check mode
 * @param           modeSet               mode to check if set
 * @param           requiredState         state required for mode to be active
 * 
 * @return          this function does not produce a return value
 */
void WebServer::calculateSocketState(unsigned int socketIndex, unsigned int sensorIndex, int modeSet, int requiredState) {

  // feed pause active and current socket is affected?
  if (feedPause && sockets[socketIndex].affectedByFeedPause) {

    // do nothing
    return;
  }

  // current mode equals the mode set and the sensor state equals the required state
  // socket not overridden
  if ((sockets[socketIndex].mode == modeSet) && (sensors[sensorIndex].state == requiredState) && (!sockets[socketIndex].override)) {

    // turn the socket on 
    toggleSocket(socketIndex, true);

    // set timestamp in seconds
    sockets[socketIndex].lastSwitchTimestamp = millis() / 1000;

    // reset to unchanged, preventing unnecessary switching
    // sensor switching only takes place if the stateChanged parameter is set to CHANGED
    sensors[sensorIndex].stateChanged = SENSORSTATE_UNCHANGED;  
        
  // curent mode equals the mode set but the sensor state is not right 
  // socket not overridden
  } else if ((sockets[socketIndex].mode == modeSet) && (!sockets[socketIndex].override)) {

    // turn the socket off
    toggleSocket(socketIndex, false);
 
    // set timestamp in seconds
    sockets[socketIndex].lastSwitchTimestamp =  millis() / 1000;

    // reset to unchanged, preventing unnecessary switching
    // sensor switching only takes place if the stateChanged parameter is set to CHANGED
    sensors[sensorIndex].stateChanged = SENSORSTATE_UNCHANGED;  
  }     
}

/**
 * Switches the hardware sockets according to socket and sensor states
 * 
 * @return          this function does not produce a return value
 */
void WebServer::switchSockets() {

  // iterate through all sockets
  for (unsigned int iterator = 0; iterator < 5; iterator ++) {

    // socket not overridden
    if (!sockets[iterator].override && !(sockets[iterator].affectedByFeedPause && feedPause)) {

      // socket mode always off?
      if (sockets[iterator].mode == SOCKET_MODE_ALWAYS_OFF) {
         
         // turn off
         toggleSocket(iterator, false);
      }

      // socket mode always on?
      if (sockets[iterator].mode == SOCKET_MODE_ALWAYS_ON) {

        // turn on
        toggleSocket(iterator, true);
      }  
    }
  }

  // iterate through all sensors
  for (unsigned int iterator = 0; iterator < 5; iterator ++) {

    // current sensor state changed?
    if (sensors[iterator].stateChanged == SENSORSTATE_CHANGED) {
      
      // switch statement for sensor iterator, corresponding to
      // the sensor numbers
      switch (iterator) {

        // sensor 0: pH 1
        case 0: {

          // iterate through all sockets
          for (unsigned int socketIterator = 0; socketIterator < 5; socketIterator ++) {

            // calculate socket states
            calculateSocketState(socketIterator, iterator, SOCKET_MODE_PH1_LOW, SENSORSTATE_LOW);
            calculateSocketState(socketIterator, iterator, SOCKET_MODE_PH1_HIGH, SENSORSTATE_HIGH);
          }  
        } break;

        // sensor 1: pH 2
        case 1: {

          // iterate through all sockets
          for (unsigned int socketIterator = 0; socketIterator < 5; socketIterator ++) {

            // calculate socket states
            calculateSocketState(socketIterator, iterator, SOCKET_MODE_PH2_LOW, SENSORSTATE_LOW);
            calculateSocketState(socketIterator, iterator, SOCKET_MODE_PH2_HIGH, SENSORSTATE_HIGH);
          }
        } break;

        // sensor 2: temp 1
        case 2: {

          // iterate through all sockets
          for (int socketIterator = 0; socketIterator < 5; socketIterator ++) {

            // calculate socket states
            calculateSocketState(socketIterator, iterator, SOCKET_MODE_TEMP1_LOW, SENSORSTATE_LOW);
            calculateSocketState(socketIterator, iterator, SOCKET_MODE_TEMP1_HIGH, SENSORSTATE_HIGH);     
          }      
        } break;

        // sensor 3: temp 2
        case 3: {

          // iterate through all sockets
          for (int socketIterator = 0; socketIterator < 5; socketIterator ++) {

            // calculate socket states
            calculateSocketState(socketIterator, iterator, SOCKET_MODE_TEMP2_LOW, SENSORSTATE_LOW);
            calculateSocketState(socketIterator, iterator, SOCKET_MODE_TEMP2_HIGH, SENSORSTATE_HIGH); 
          }      
        } break;

        // sensor 4: level switch
        case 4: {

          // iterate through all sockets
          for (int socketIterator = 0; socketIterator < 5; socketIterator ++) {

            // calculate socket states
            calculateSocketState(socketIterator, iterator, SOCKET_MODE_LEVEL_LOW, SENSORSTATE_LOW);
            calculateSocketState(socketIterator, iterator, SOCKET_MODE_LEVEL_HIGH, SENSORSTATE_HIGH);
          }
        } break;
      }
    } else if (feedPause) {

      // iterator through all sockets
      for (unsigned int socketIterator = 0; socketIterator < 5; socketIterator ++) {

        // affected by feed pause and not overridden
        if (sockets[socketIterator].affectedByFeedPause && !sockets[socketIterator].override) {
         
          // turn the socket off
          toggleSocket(socketIterator, false);
        }
      }     
    }
  }

  // detect warnings (sockets which are switched on longer than the maximum time specified)
  detectSocketWarnings();
}

/**
 * Detects socket warnings (alarms) by checking if the sockets are not longer activated
 * than specified in the max on time
 * 
 * @return          this function does not produce a return value
 */
void WebServer::detectSocketWarnings() {

  // iterate through all sockets
  for (int iterator = 0; iterator < 5; iterator ++) {

     // only check sockets with maxOnTime parameter set (0 = disabled)
     if ((sockets[iterator].maxOnTime > 0)) {

        // if max time passed, socket is on and alarm not yet raised
        if ((sockets[iterator].maxOnTime < ((millis() / 1000) - sockets[iterator].lastSwitchTimestamp)) && (sockets[iterator].isActive) && (!sockets[iterator].alarmRaised)) {

          // toggle socket off
          toggleSocket(iterator, false);
          
          // set alarm to true;
          sockets[iterator].alarmRaised = true;
        } 
     }   
  } 
}

/**
 * Forces socket switching, even though sensors are not changed
 * Is used to reset in case the microprocessor fails and resets.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::forceSocketSwitching() {

  // iterate through all sensors
  for (unsigned int iterator = 0; iterator < 5; iterator ++) {

    // set sensor state to changed
    sensors[iterator].stateChanged = SENSORSTATE_CHANGED;
  }
}

/**
 * Calculates sensor states using sensor current values and set alarm values
 * 
 * @return          this function does not produce a return value
 */
void WebServer::calculateSensorState() {

  // iterate through all sensors
  for (unsigned int iterator = 0; iterator < 5; iterator ++) {

    // auto recovery enabled?
    if (alarmAutoRecovery) {
      
      // reset alarm for current sensor
      sensors[iterator].alarmRaised = false;
    }
  }

  // iterate through ph and temperature sensors (sensor 0 - 3)
  for (unsigned int iterator = 0; iterator < 4; iterator ++) {

    // current value lower than low alarm setting or higher than high alarm setting
    if ((sensors[iterator].getValue() <= sensors[iterator].lowAlarm) || (sensors[iterator]. getValue() >= sensors[iterator].highAlarm)) {

      // sensor enabled?
      if (sensors[iterator].enabled) {

        // raise alarm for current sensor
        sensors[iterator].alarmRaised = true;     
      } else {

        // disable alarm
        sensors[iterator].alarmRaised = false;
      }
    }   
  }

  // level switch alarm set to low and currently low
  if ((sensors[4].lowAlarm == 100) && (sensors[4].getValue() == 0)) {
    
    // raise alarm
    sensors[4].alarmRaised = true;
  }

  // level switch alarm set to high and currently high
  if ((sensors[4].highAlarm == 100) && (sensors[4].getValue() == 100)) {

    // raise alarm
    sensors[4].alarmRaised = true;
  }

  // iterate through ph and temperature sensors
  for (unsigned int iterator = 0; iterator < 4; iterator ++) {

    // sensor disabled?
    if (!sensors[iterator].enabled) {

        // if disabled, sensorstate is always ok!
        sensors[iterator].state = SENSORSTATE_OK;
        sensors[iterator].stateChanged = SENSORSTATE_CHANGED;
        sensors[iterator].alarmRaised = false;
    
    // current state is low
    } else if (sensors[iterator].state == SENSORSTATE_LOW) {

      // value higher than low value plus threshold?
      if (sensors[iterator].getValue() >= (sensors[iterator].lowValue + sensors[iterator].threshold)) {
       
        // set sensor state to ok 
        sensors[iterator].state = SENSORSTATE_OK;
        sensors[iterator].stateChanged = SENSORSTATE_CHANGED;
      }

    // current state is high
    } else if (sensors[iterator].state == SENSORSTATE_HIGH) {

      // current value is less than highvalue minus threshold?
      if (sensors[iterator].getValue() <= (sensors[iterator].highValue - sensors[iterator].threshold)) {
        
        // set sensor state to ok
        sensors[iterator].state = SENSORSTATE_OK;
        sensors[iterator].stateChanged = SENSORSTATE_CHANGED;
      }

    // current state is ok
    } else if (sensors[iterator].state == SENSORSTATE_OK) {

      // current value is less than or equal to low value?
      if (sensors[iterator].getValue() <= sensors[iterator].lowValue) {
        
        // set sensor state to low
        sensors[iterator].state = SENSORSTATE_LOW;
        sensors[iterator].stateChanged = SENSORSTATE_CHANGED;

      // current value is more than or equal to high value
      } else if (sensors[iterator].getValue() >= sensors[iterator].highValue) {

        // set sensor state to high
        sensors[iterator].state = SENSORSTATE_HIGH;
        sensors[iterator].stateChanged = SENSORSTATE_CHANGED;
      } 
    }  
  }

  // level switch low and current state high?
  if ((sensors[4].getValue() < 100) && (sensors[4].state != SENSORSTATE_LOW)) {
    
    // set sensor state to low
    sensors[4].state = SENSORSTATE_LOW;
    sensors[4].stateChanged = SENSORSTATE_CHANGED;
  }

  // level switch high and current state low?
  if ((sensors[4].getValue() > 0) && (sensors[4].state != SENSORSTATE_HIGH)) {
    
    // set sensor state to high
    sensors[4].state = SENSORSTATE_HIGH;
    sensors[4].stateChanged = SENSORSTATE_CHANGED;
  }       
}

/**
 * Lists all available Wifi networks
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::listNetworks() {

  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // authorized
  } else {

    // set status to ok 
    jsonObject.set("status", "ok");

    // get the number of available networks
    int networkCount = WiFi.scanNetworks();

    // create an array to store all network names
    JsonArray& jsonArray = jsonBuffer.createArray();

    // iterate the number of networks
    for (unsigned int iterator = 0; iterator < networkCount; iterator ++) {

      // add the current ssid to the array
      jsonArray.add(WiFi.SSID(iterator));
    }

    // add networks array to the json object
    jsonObject.set("networks", jsonArray);
  }
  
  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);

  // output json to client
  outputJSON(jsonOutput);
}

/**
 * Post processes the i2c data, overwriting the received sensor
 * values with averaged ones.
 * 
 * @param           data          data array received on i2c bus
 * 
 * @return          this function does not produce a return value
 */
void WebServer::postProcessSensorValues(byte data[16]) {

  // iterate through all but the level sensor (sensorindex = 4)
  for (unsigned int iterator = 0; iterator < 4; iterator ++) {

    // overwrite sensor value
    data[(iterator * 2) + 1] = sensorValues[iterator] >> 8;
    data[(iterator * 2) + 2] = sensorValues[iterator] & 255;

    // set alarm raised parameter for current sensor to false;
    sensors[iterator].alarmRaised = false;

    // save enabled/disabled state
    sensors[iterator].enabled = (data[0] >> iterator) & 1;
        
    // sensor data active and data integrity ok
    if (active && i2cComm.checkDataIntegrity(data) && sensors[iterator].enabled) {
      
      // alarm values set?
      if ((sensors[iterator].lowAlarm > 0) && (sensors[iterator].highAlarm > 0)) {
  
        // calculate alarm state
        if ((sensorValues[iterator] <= sensors[iterator].lowAlarm) || (sensorValues[iterator] >= sensors[iterator].highAlarm)) {
  
          // set alarm state to true
          sensors[iterator].alarmRaised = true;
        }
      }
    }
  }

  // level alarm is not calculated but simply copied
  // from the original data
  bool levelAlarm = (data[11] >> 4) & 1;

  // alter the alarm byte
  data[11] = (levelAlarm << 4) + (sensors[3].alarmRaised << 3) + (sensors[2].alarmRaised << 2) + (sensors[1].alarmRaised << 1) + sensors[0].alarmRaised;
}

/**
 * Post processes the i2c data, storing sensor settings 
 * 
 * @param           sensorIndex   sensor index to store values for
 * @param           data          data array received on i2c bus
 * 
 * @return          this function does not produce a return value
 */
void WebServer::postProcessSensorSettings(unsigned int sensorIndex, byte data[16]) {

  // store low and high alarms
  sensors[sensorIndex].lowAlarm = (data[0] << 8) + data[1];
  sensors[sensorIndex].highAlarm = (data[2] << 8) +  data[3];
  
}


/**
 * Executes a query on the I2C bus. Any command can be executed as long
 * as the user is logged in. To execute a query a 17 byte data array
 * is required passed to the HTML server as GET argument with
 * name 'data'. e.g. http://pondctrl.local/?data=%35%00%00[..].
 * as shown above, each byte has to be coded in the url. The query
 * result will be given as output, without further processing.
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */
void WebServer::query() {
  
  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // authorized
  } else {

    // required data parameter not given?
    if (!moduleWebServer.hasArg("data")) {

      // set status parameter to no, errno to 2 (empty request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 2);

    // data argument given
    } else {

      // data argument length wrong
      if (moduleWebServer.arg("data").length() != 17) {

        // set status parameter to no, errno to 3 (invalid request)
        jsonObject.set("status", "no");
        jsonObject.set("errno", 3);

      // data argument length correct
      } else {

        // create a string from the data arqument
        String myquery = moduleWebServer.arg("data");

        // two byte arrays to store data to and data from the WiFi module
        byte data[16];
        byte i2cData[16];

        // byte to store I2C command
        byte commandToPondCTRL;

        // iterate the full query (data parameter) length
        for (int iterator = 0 ; iterator < myquery.length(); iterator++) {

          // get the byte from the current location
          byte currentByte = (byte) myquery.charAt(iterator);

          // iteration higher than zero
          if (iterator > 0) {

            // data byte

            // store the byte in the data to wifi module array
            data[iterator - 1] = currentByte;

          // iterator equals zero
          } else {

            // command byte
            
            // store the command byte
            commandToPondCTRL = currentByte;
          }

        }

        // execute the command on the I2C bus
        i2cComm.processCommand(commandToPondCTRL, data, i2cData);

        // set status parameter to ok
        jsonObject.set("status", "ok");

        // create an array to store each of the 16 received
        // bytes from the I2C slave
        JsonArray& jsonArray = jsonBuffer.createArray();

        // user requests sensor values
        // sensor values are overwritten with average values
        // from last ten readings. this is implemented because some
        // cheap sensors are fluctuating.
        if (commandToPondCTRL == I2C_GET_SENSOR_VALUES) {
          
          // post process sensor values
          // meaning: overwrite with averaged values
          postProcessSensorValues(i2cData);
        }      

        // user requests sensor settings?
        // sensor settings are used to calculate actual alarm settings
        // the alarm settings are overwritten from the values, received from pondctrl
        if ((commandToPondCTRL >= I2C_GET_SENSOR_PH1_SETTINGS) && (commandToPondCTRL <= I2C_GET_SENSOR_LEVEL_SETTINGS)) {

          // post process sensor settings
          // meaning: set sensor object values
          postProcessSensorSettings((commandToPondCTRL - I2C_GET_SENSOR_PH1_SETTINGS), i2cData);
        }

        // iterate through the received byters (16)
        for (char dataIterator = 0; dataIterator < 16; dataIterator ++) {
          
          // add the byte to the array
          jsonArray.add((int)i2cData[dataIterator]);
        
        }

        // add the byte array to the json object
        jsonObject.set("bytes", jsonArray);
      }      
    }   
  }

  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);

  // output json to client
  outputJSON(jsonOutput);
}

/**
 * Shows the hard-coded web page as built-in interface (index.h)
 * 
 * @return          this function does not produce a return value
 */
void WebServer::showInterface() {

  // allow cross resource sharing 
  moduleWebServer.sendHeader("Access-Control-Allow-Origin", "*");

  // send http response code 200 and the content of index.h 
  // MAIN_page is declared in index.h
  moduleWebServer.send_P(200, "text/html", MAIN_page, sizeof(MAIN_page));
}


/**
 * Sets the administrative account credentials.
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */ 
void WebServer::setAdmin() {

  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // not autorized
  if (!isAuthorized()) {

    // set status parameter to no, errno to 1 (login required)
    jsonObject.set("status", "no");
    jsonObject.set("errno", 1);

  // authorized
  } else {

    // no username and password argument specified?
    if (!moduleWebServer.hasArg("username") || !moduleWebServer.hasArg("password")) {

      // set status to no, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);

    // username or password arguments shorter than 8 characters?
    } else if ((moduleWebServer.arg("username").length() < 8) || (moduleWebServer.arg("password").length() < 8)) {

      // set status to nu, errno to 3 (invalid request)
      jsonObject.set("status", "no");
      jsonObject.set("errno", 3);

    // all ok
    } else {

      // set status to ok
      jsonObject.set("status", "ok");

      // set administrative account credentials to specified parameters
      module_gui_username = moduleWebServer.arg("username");
      module_gui_password = moduleWebServer.arg("password");

      // write credentials to memory
      memTools.writeMemory(memTools.EEPROM_GUI_USERNAME, moduleWebServer.arg("username"));
      memTools.writeMemory(memTools.EEPROM_GUI_PASSWORD, moduleWebServer.arg("password"));

      // force a new login by resetting the administator ip
      administrator = IPAddress(0, 0, 0, 0);
    }  
  }
  
  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);

  // output json to client
  outputJSON(jsonOutput);
}

/**
 * Sets the administrative account credentials.
 * 
 * The function itself does not produce a return value. The generated
 * value (json string) is sent to the web server client.
 * 
 * @return          this function does not produce a return value
 */ 
void WebServer::doLogout() {

  // reset the administaror ip, forcing login
  administrator = IPAddress(0, 0, 0, 0);

  // create a json object within a json buffer
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // we do not check if the user is really logged in
  // to save program space. logging out while not 
  // logged in does no harm
  
  // set status to ok
  jsonObject.set("status", "ok");
    
  // output json output to string
  String jsonOutput;
  jsonObject.printTo(jsonOutput);

  // output json to client
  outputJSON(jsonOutput);
}

/**
 * Set the latest firmware version 
 * 
 * @param           versionNumber             latest version number
 * 
 * @return          this function does not produce a return value
 */ 
void WebServer::setFirmwareUpdateVersion(unsigned int versionNumber) {

  // set the latest firmware version variable
  firmwareUpdateVersion = versionNumber;
}

/**
 * Set the current firmware version
 * 
 * @param           versionNumber             current version number
 * 
 * @return          this function does not produce a return value
 */ 
void WebServer::setFirmwareVersion(unsigned int versionNumber) {
  
  // set the current firmware version variable
  currentFirmwareVersion = versionNumber;
}

/**
 * Set the current WiFi module ssid, passphrase and mdns hostname
 * 
 * @param           targetSSID             ssid to connect with
 * @param           targetPassphrase       passphrase for specified network
 * @param           mdnsHostname           mdns responder hostname
 * 
 * @return          this function does not produce a return value
 */ 
void WebServer::setSSIDPassPhraseAndMDSNHostname(String targetSSID, String targetPassphrase, String mdnsHostname) {
  
  // set ssid, passphrase and hostname
  module_ssid = targetSSID;
  module_passphrase = targetPassphrase;
  module_mdns_hostname = mdnsHostname;
}

/**
 * Sets the current WiFi mode
 * 
 * @param           mode                current wireless mode (local ap = 1, client = 2)
 * 
 * @return          this function does not produce a return value
 */ 
void WebServer::setWirelessMode(char mode) {
  
  // set wireless mode
  module_wireless_mode = mode;
}

/**
 * Processes web requests to the web server
 *
 * @return          this function does not produce a return value
 */ 
void WebServer::handleRequests() {

  // process web server client requests
  moduleWebServer.handleClient();
}

/**
 * Set other settings 
 * 
 * @param                 apiKeyFromEEPROM              module api key
 * @param                 alarmSound                    alarm sound (1 = on, 0 = false)
 * @param                 alarmAutoReset                alarm auto recovery (1 = on, 0 = false)
 * @param                 feedpauseDurationFromEEPROM   feed pause duration in seconds
 * @param                 timezoneOffsetFromEEPROM      timezone offset from UTC in seconds
 * @param                 timeServer                    timeserver address
 * 
 * @return                this function does not produce a return value
 */
void WebServer::setOtherSettings(String apiKeyFromEEPROM, unsigned int alarmSound, unsigned int alarmAutoReset, unsigned int feedpauseDurationFromEEPROM, int timezoneOffsetFromEEPROM, String timeserverFromEEPROM, unsigned int upnpPortFromEEPROM) {

  // set api key
  apiKey = apiKeyFromEEPROM;

  // set feedpause duration
  feedpauseDuration = feedpauseDurationFromEEPROM;

  // set timezone offset
  timezoneOffset = timezoneOffsetFromEEPROM;

  // set time server
  timeserver = timeserverFromEEPROM;

  // set port number
  upnpPort = upnpPortFromEEPROM;

  // alarmSound is set to 1?
  if (alarmSound == 1) {

    // yes, set sound on to true
    alarmSoundOn = true;

  } else {

    // no, set sound on to false
    alarmSoundOn = false;
  }

  // alarm auto reset set to 1?
  if (alarmAutoReset == 1) {

    // yes, set alarm auto recovery to true
    alarmAutoRecovery = true;
    
  } else {

    // no, set alarm auto recovery to false
    alarmAutoRecovery = false;
  }
}

/**
 * Constructor
 *
 * @return          this function does not produce a return value
 */ 
WebServer::WebServer() {

  // set sensor 4 to switch mode (no value averaging)
  sensors[4].isSwitch = true;

  // set all tarhet functions for http requests
  moduleWebServer.on("/get", HTTP_GET, std::bind(&WebServer::get, this));
  moduleWebServer.on("/toggleFeedPause", HTTP_POST, std::bind(&WebServer::toggleFeedPause, this));
  moduleWebServer.on("/toggleAlarmSound", HTTP_POST, std::bind(&WebServer::toggleAlarmSound, this));
  moduleWebServer.on("/toggleAlarmAutoRecovery", HTTP_POST, std::bind(&WebServer::toggleAlarmAutoRecovery, this));
  moduleWebServer.on("/setAdmin", HTTP_POST, std::bind(&WebServer::setAdmin, this));
  moduleWebServer.on("/doLogout", HTTP_GET, std::bind(&WebServer::doLogout, this));
  moduleWebServer.on("/doUpdate", HTTP_GET, std::bind(&WebServer::doUpdate, this));
  moduleWebServer.on("/listNetworks", HTTP_GET, std::bind(&WebServer::listNetworks, this)); 
  moduleWebServer.on("/doLogin", HTTP_POST, std::bind(&WebServer::doLogin, this));
  moduleWebServer.on("/setWiFi", HTTP_POST, std::bind(&WebServer::setWiFi, this));
  moduleWebServer.on("/setApiKey", HTTP_POST, std::bind(&WebServer::setApiKey, this));
  moduleWebServer.on("/query", HTTP_GET, std::bind(&WebServer::query, this));  
  moduleWebServer.on("/getMac", HTTP_GET, std::bind(&WebServer::getMac, this));
  moduleWebServer.on("/", HTTP_GET, std::bind(&WebServer::showInterface, this));    
  moduleWebServer.on("/setSensor", HTTP_POST, std::bind(&WebServer::setSensor, this));
  moduleWebServer.on("/setSocket", HTTP_POST, std::bind(&WebServer::setSocket, this));
  moduleWebServer.on("/setSystem", HTTP_POST, std::bind(&WebServer::setSystem, this));
  moduleWebServer.on("/setPHSensorCalibration", HTTP_POST, std::bind(&WebServer::setPHSensorCalibration, this));
  moduleWebServer.on("/setTemperatureSensorCalibration", HTTP_POST, std::bind(&WebServer::setTemperatureSensorCalibration, this));
  moduleWebServer.on("/reset", HTTP_GET, std::bind(&WebServer::reset, this));

  // start the web server
  moduleWebServer.begin();
}
