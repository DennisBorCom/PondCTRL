/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */

#include "iot.h"

/**
 * Uploads data to the Pond[CTRL]™ api
 * 
 * @param           sensors           sensor array   
 * @param           sockets           socket array
 * @param           firmwareVersion   current firmware version
 * @param           currentTimestamp  WiFi-module running time
 * 
 * @return          returns the server response (JSON)
 */
String IOTLink::processData(Sensor sensors[5], Socket sockets[5], unsigned int firmwareVersion, unsigned long currentTimestamp, unsigned int externalPort, float latitude, float longitude) {

  // create a json object in a json buffer 
  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonObject = jsonBuffer.createObject();

  // set macaddress, local ip and firmware version parameters
  jsonObject.set("macaddress", WiFi.macAddress());
  jsonObject.set("localip", WiFi.localIP().toString());
  jsonObject.set("firmware", firmwareVersion);

  // set uptime and port number
  jsonObject.set("uptime", currentTimestamp);
  jsonObject.set("portnumber", externalPort); 

  // set debug information, for now this is the restart reason
  jsonObject.set("debug", String(ESP.getResetInfoPtr()->reason));

  // create arrays within the jsonbuffer for sensors and sockets
  JsonArray& jsonSensorArray = jsonBuffer.createArray();
  JsonArray& jsonSocketArray = jsonBuffer.createArray();

  // iterate five times (5 sensors and 5 sockets)
  for (unsigned int iterator = 0; iterator < 5; iterator ++) {

      // create a json object for the sensor
      JsonObject& jsonSensor = jsonBuffer.createObject();
      
      // set id, name, currentvalue, alarmraised, enabled, lowvalue, highvalue, lowalarm and highalarm
      jsonSensor.set("id", iterator);
      jsonSensor.set("name", sensors[iterator].name);
      jsonSensor.set("currentvalue", sensors[iterator].getValue());
      jsonSensor.set("alarmraised", sensors[iterator].alarmRaised);
      jsonSensor.set("enabled", sensors[iterator].enabled);
      jsonSensor.set("lowvalue", sensors[iterator].lowValue);
      jsonSensor.set("highvalue", sensors[iterator].highValue);
      jsonSensor.set("lowalarm", sensors[iterator].lowAlarm);
      jsonSensor.set("highalarm", sensors[iterator].highAlarm);

      // add the sensor object to the sensor array
      jsonSensorArray.add(jsonSensor);

      // create a json object for the socket
      JsonObject& jsonSocket = jsonBuffer.createObject();

      // set id, name, currentstate and alarmraised
      jsonSocket.set("id", iterator);
      jsonSocket.set("name", sockets[iterator].name);
      jsonSocket.set("currentstate", sockets[iterator].isActive);
      jsonSocket.set("alarmraised", sockets[iterator].alarmRaised);

      // add the socket object to the socket array
      jsonSocketArray.add(jsonSocket);         
  }

  // add sensor array as parameter
  jsonObject.set("sensors", jsonSensorArray);

  // add socket array as parameter
  jsonObject.set("sockets", jsonSocketArray);

  // add gps coordinates as parameters
  jsonObject.set("latitude", latitude);
  jsonObject.set("longitude", longitude);

  // empty string to store json string
  String jsonString;

  // print the json object to the string
  jsonObject.printTo(jsonString);

  // create a new httpclient object
  // currently the firmware only supports http and no https because of the required amount
  // of additional code required to verify https authentication and the limitation of 
  // 512kb firmware size without losing OTA update capability.
  HTTPClient http;

  // open the api url
  http.begin("http://pondctrl.dennisbor.com/api/");

  // set request timeout to 15 seconds
  http.setTimeout(15000);

  // post json data
  http.POST(jsonString);

  // store html result output
  String htmlOutput = http.getString();

  // end http transmission
  http.end();

  // please note that no error handling is added. if not connected to an access point
  // or the access point has no working internet connection, there is simply no
  // data uploaded to the api.

  // result html output data
  return htmlOutput;
}
