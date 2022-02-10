/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 *
 * Required ArduinoJson Version 5.13.4 and custom TinyUPnP (with both internal and external port support, can by acquired on request
 * at dennis@dennisbor.com).
 */

// external libraries, please note that TinyUPnP required a custom version (see description above)
#include "TinyUPnP.h"
#include <ESP8266mDNS.h>
#include "DNSServer.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

// own libraries
#include "memory.h"
#include "i2c.h"
#include "socket.h"
#include "sensor.h"
#include "iot.h"
#include "webserver.h"

#undef UPNP_DEBUG

#define debugPrint(...)

// current firmware version
const int FIRMWARE_VERSION = 1223;

// Dns server object. in AP mode (when WiFi not configured yet or
// connection to known network is lost), the firmware starts a DNS
// server to forward all traffic to the module's webinterface for
// WiFi configuration
DNSServer dnsServer;

// default AP address
IPAddress accessPointIPAddress(192,168,4,1);

// TinyUPnP object, to support UPnP (for enabled routers). 
// Either UPnP or port forwarding is required to be able to use
// the online Pond[CTRL]™ dashboard
TinyUPnP *tinyUPnP = new TinyUPnP(20000); 

// strings to hold ssid, passphrase and mdns responder hostname
// when loaded from EEPROM
String ssid, passphrase, mdns_hostname;

// variables to store current current, last run timestamps and last time 
// values obtained timestamp
unsigned long currentTimestamp, lastRunTimestamp, valueTimestamp;

// byte array to store received I2C data. the WiFi-module acts
// as the I2C master, sending a command byte followed by 16
// data bytes (17 bytes total). Pond[CTRL]™ responds with 16
// data bytes. 
byte i2cData[16];

// data array for unused data
byte unused[16];

// variable to store the total number of iterations of the main
// loop. used for uploading data to the Pond[CTRL]™ dashboard
// every 12 iterations (loop is executed every 5 seconds, times
// 12 iterations, one data upload every 60 seconds).
unsigned long iterations = 0;

// create objects from own classes
WebServer webserver;
I2C i2cComm;
IOTLink internetOfThings;
MemoryTools memTools;

// variables for time server synchronization
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String currentTimeserver = "ntp.pool.org";

/**
 * Maintains the connection and solves possible connection drops
 * by attempting to connect to the specified network or otherwise
 * starting an AP
 * 
 * @return          this function does not produce a return value
 */
void maintainConnection() {
    
    // attempt to connect to specified access point.
    // if not succesful (false)...
    if (!connectAP()) {
      
      // start an access point
      startAP();
    }
}

/**
 * Connects to the specified WiFi network
 * 
 * @return          true if connected, otherwise false
 */
bool connectAP() {

  // no ssid set?
  if (ssid == "") {

    // return false
    return false;
  }

  // standalone wifi mode (client, no ap)
  WiFi.mode(WIFI_STA);

  // set the device hostname
  WiFi.hostname("pondctrl");

  // connect to the specified access point using specified ssid and passphrase  
  WiFi.begin(
    ssid.c_str(),
    passphrase.c_str()
  );

  // iterator for a maximum of <defined> connection attempts
  int iterator = 0;
  
  // iterate as long as not connected we did not iterate
  // 10 times yet. between iterations there is a 5s delay
  while ((WiFi.status() != WL_CONNECTED) && (iterator < 10)) {

      // wait for 1s
      delay(5000);

      // increment the iterator
      iterator ++;
  }

  // still not connected (25 unsuccesful connection attempts)
  if (WiFi.status() != WL_CONNECTED) {

    // return false
    return false;

  // connected to given network!
  } else {

    // disconnect the local access point
    WiFi.softAPdisconnect(true);

    // set wireless mode to client mode
    webserver.setWirelessMode(2);

    // all set!

    // initialize upnp port mappings
    setUPnPMappings();

    // return true
    return true;
  }
}

/**
 * Sets the UPnP port mappings required for the Pond[CTRL]™ Dashboard. 
 * Mappings are from external port 7777 to internal port 80 with an alive time of 3600 seconds
 *
 * @return          this function does not produce a return value
 */
void setUPnPMappings() {

    // add a mapping from the current from the specified external port to internal port 80, interval of 300
    // seconds and description Pond[CTRL]
    tinyUPnP->addPortMappingConfig(WiFi.localIP(), 80, webserver.getUPnPPort(), RULE_PROTOCOL_TCP, 300, "Pond[CTRL]");
    
    // commit port mappings to the router
    tinyUPnP->commitPortMappings();
}

/**
 * Starts an access point
 * 
 * @return          this function does not produce a return value
 */
void startAP() {

  // set wifi mode to access point
  WiFi.mode(WIFI_AP);

  // wait 500ms to continue
  // this delay is added because without it, starting the access point
  // would fail regularly 
  delay(500);

  // start a new access point with name Pond[CTRL] by Dennis Bor
  // if succeeded...
  if (WiFi.softAP("Pond[CTRL] by Dennis Bor", "")) {
    
    // set the wireless mode to access point (1)
    webserver.setWirelessMode(1);
  } 

  // please not that there is no 'what if we could not start an access 
  // point'. every five seconds, the firmware will try to reconnect
  // to an existing network or reattempt to set up an access point.
}

/**
 * Sets up all important stuff
 *
 * @return          this function does not produce a return value
 */
void setup(void) {

  // setup the dns server to capture all traffic if in ap mode
  // so the user interface acts as captive portal
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", accessPointIPAddress);

  // start EEPROM
  EEPROM.begin(4096);

  // start seriel at 115200 baud rate
  Serial.begin(115200);

  // show debug information to serial
  Serial.print(F("Pond[CTRL] WiFi module firmware by Dennis Bor, version "));
  Serial.println(FIRMWARE_VERSION);

  // check if this is the first run. if not, memory will be programmed 
  // to default values
  memTools.checkFirstRun();

  // read ssid, passphrase and mdns responder name from memory
  ssid = memTools.readMemory(memTools.EEPROM_SSID);
   
  passphrase = memTools.readMemory(memTools.EEPROM_PASSPHRASE);
  
  mdns_hostname = memTools.readMemory(memTools.EEPROM_MDNSRESPONDER);  

  // add and start a mdns responder for tcp port 80
  MDNS.addService("http","tcp",80);
  MDNS.begin(mdns_hostname, WiFi.localIP());

  // start connection (client if network set, otherwise access point)
  maintainConnection();

  // calculate timezone offset using EEPROM data
  // offset is stored in 4 bytes:
  // bytes 0-1: positive offset value in seconds
  // bytes 2-3: negative offset value in seconds
  String timezoneOffsetFromEEPROM = memTools.readMemory(memTools.EEPROM_TIMESERVER_UTC_OFFSET);

  // calculate timezone offset
  int timezoneOffset = ((memTools.readCharFromMemory(memTools.EEPROM_TIMESERVER_UTC_OFFSET[0]) << 8) + memTools.readCharFromMemory(memTools.EEPROM_TIMESERVER_UTC_OFFSET[0] + 1)) - ((memTools.readCharFromMemory(memTools.EEPROM_TIMESERVER_UTC_OFFSET[0] + 2) << 8) + memTools.readCharFromMemory(memTools.EEPROM_TIMESERVER_UTC_OFFSET[0] + 3));

  // get feedpause duration from memory
  unsigned int feedpauseDuration = (memTools.readCharFromMemory(memTools.EEPROM_FEEDPAUSE_DURATION[0]) << 8) + (memTools.readCharFromMemory(memTools.EEPROM_FEEDPAUSE_DURATION[0] + 1));

  // get current timeserver
  currentTimeserver =  memTools.readMemory(memTools.EEPROM_TIMESERVER_ADDRESS);

  // get upnp port from memory
  unsigned int upnpPort = (memTools.readCharFromMemory(memTools.EEPROM_UPNP_PORT[0]) << 8) + (memTools.readCharFromMemory(memTools.EEPROM_UPNP_PORT[0] + 1));

  // pass all required data to webserver
  webserver.setSSIDPassPhraseAndMDSNHostname(ssid, passphrase, mdns_hostname);
  webserver.setGUICredentials(memTools.readMemory(memTools.EEPROM_GUI_USERNAME), memTools.readMemory(memTools.EEPROM_GUI_PASSWORD));
  webserver.setOtherSettings(memTools.readMemory(memTools.EEPROM_APIKEY), memTools.readMemory(memTools.EEPROM_ALARM_SOUND).toInt(), memTools.readMemory(memTools.EEPROM_ALARM_AUTORESET).toInt(), feedpauseDuration, timezoneOffset, currentTimeserver, upnpPort);
  webserver.setFirmwareVersion(FIRMWARE_VERSION);

  // set debug information (for now: reset reason)
  webserver.setCustomData(String(ESP.getResetInfoPtr()->reason));
  
  // check for updates
  webserver.getFirmwareUpdateVersion();

  // initialize empty data array to pass to i2c slave
  byte data[16];
      
  // start i2c communication
  Wire.begin(0, 2);
 
  // delay of 1s to prevent temperature reading errors 
  delay(1000);

  // turn off programmable led indicating there is no active IOT link yet
  i2cComm.processCommand(I2C_SET_LED_OFF, data, i2cData);   

  // array with 20 commands to request from Pond[CTRL]™ before we can upload
  // data to the api. data collected are sensor names, values settings and
  // alarms and socket names, settings and alarms.
  byte requiredData[20] = { I2C_GET_SENSOR_PH1_NAME, I2C_GET_SENSOR_PH2_NAME, I2C_GET_SENSOR_TEMP1_NAME, I2C_GET_SENSOR_TEMP2_NAME, I2C_GET_SENSOR_LEVEL_NAME, I2C_GET_SOCKET1_NAME, I2C_GET_SOCKET2_NAME, I2C_GET_SOCKET3_NAME, I2C_GET_SOCKET4_NAME, I2C_GET_SOCKET5_NAME, I2C_GET_SOCKET1_SETTINGS, I2C_GET_SOCKET2_SETTINGS, I2C_GET_SOCKET3_SETTINGS, I2C_GET_SOCKET4_SETTINGS, I2C_GET_SOCKET5_SETTINGS, I2C_GET_SENSOR_PH1_SETTINGS, I2C_GET_SENSOR_PH2_SETTINGS, I2C_GET_SENSOR_TEMP1_SETTINGS, I2C_GET_SENSOR_TEMP2_SETTINGS, I2C_GET_SENSOR_LEVEL_SETTINGS };

  // iterate through all 21 commands
  for (unsigned int iterator = 0; iterator < 20; iterator ++) {

     // iterate 16 times to clear the byte array
     for (unsigned int byteIterator = 0; byteIterator < 16; byteIterator ++) {
          
          // set current byte value to zero
          i2cData[byteIterator] = 0;
     }

     // send request to i2c bus
     i2cComm.processCommand(requiredData[iterator], unused, i2cData, webserver.sensors, webserver. sockets);

      // data integrity check failed?
      if (!i2cComm.checkDataIntegrity(i2cData)) {

          // decrement the iterator, to process the same
          // request once more
          iterator -= 1;
      }

      // delay of 10ms to make sure the i2c bus
      // is ready again
      delay(10);
  }
 
  // begin timeserver synchronization
  timeClient.begin(); 
}

/**
 * Main loop
 * 
 * @return          this function does not produce a return value
 */
void loop() {
 
  // update mdns responder
  MDNS.update();

  // process next dns server request
  dnsServer.processNextRequest();

  // process webserver requests
  webserver.handleRequests();

  // current timestamp (since start, not since unix epoch)
  currentTimestamp = millis();

  // one second passes since last retrieval of sensor data
  if (currentTimestamp > (valueTimestamp + 1000)) {

    // retrieve sensor data
    i2cComm.processCommand(I2C_GET_SENSOR_VALUES, unused, i2cData, webserver.sensors, webserver.sockets);

    // check feed pause state
    webserver.checkFeedPause();
    
    // calculate sensor states and switch sockets accordingly
    webserver.calculateSensorState();
    webserver.switchSockets();
    
    // set uptime in seconds
    webserver.setUptime((currentTimestamp / 1000));

    // set offset time
    timeClient.setTimeOffset(webserver.getTimezoneOffset());

    // timeserver updated?
    if (currentTimeserver != webserver.getTimeserver()) {

      // stop time server sync
      timeClient.end();
  
      // update timeserver variable
      currentTimeserver = webserver.getTimeserver();
      
      // set server address
      timeClient.setPoolServerName(currentTimeserver.c_str());

      // restart time server sync
      timeClient.begin();
    }

    // set unix epoch
    webserver.setTimestamp(timeClient.getEpochTime());
  
    // check data integrity
    if (i2cComm.checkDataIntegrity(i2cData)) {
    
      // set sensor values used by webserver
      webserver.setSensorValues();
  
      // set last value timestamp to current stamp
      valueTimestamp = currentTimestamp;
    }
  }

  // five seconds passed since last run
  if (currentTimestamp > (lastRunTimestamp + 5000))  {

    // if not connected
    if (WiFi.status() != WL_CONNECTED) {
        
        // setup connection. first attempt to connect to ap
        // and if faild start ap
        maintainConnection();
    }

    // update upnp port mappings
    tinyUPnP->updatePortMappings(300000, &setUPnPMappings);  // 300 seconds

    // set current stamp to last run stamp
    lastRunTimestamp = currentTimestamp;

    // force socket switching - sometimes the microprocessor
    // fails and restarts. if switching is not forced, as long
    // as sensor states do not changed, the socket will remain
    // off. this forces to reset every loop
    webserver.forceSocketSwitching();

    // connected to wifi
    if (WiFi.status() == WL_CONNECTED) {

      // initialize empty data array to pass to i2c slave
      byte data[16];

      // get special instructions from i2c slave. special instructions
      // are, for example, 'reset to factory'.
      i2cComm.processCommand(I2C_GET_SPECIAL_INSTRUCTIONS, data, i2cData);

      // iterations is dividable by 12 without a remainder: happens
      // once a minute after which data is uploaded to the Pond[CTRL]™
      // api, start after a minute (12 * 5 seconds) with uploading. this makes sure
      // the sensor values are stabilized.
      if (((iterations % 12) == 0) && (iterations > 12)) {

        // update timeserver synchronization
        timeClient.update();

        // activate sensor data calculations and postprocessing
        webserver.activateSensorData();

        // get response from iot api
        String iotResponse = internetOfThings.processData(webserver.sensors, webserver.sockets, FIRMWARE_VERSION, (currentTimestamp / 1000), webserver.getUPnPPort());

        // json buffer to store iot response json data in
        DynamicJsonBuffer iotResponseJsonBuffer;
        JsonObject& iotResponseJsonObject = iotResponseJsonBuffer.parseObject(iotResponse);

        // json parse success?
        if (iotResponseJsonObject.success()) {

          // iot status ok
          if (iotResponseJsonObject.get<String>("status") == "ok") {

            // turn on programmable led indicating there is an active IOT link
            i2cComm.processCommand(I2C_SET_LED_ON, data, i2cData);

          // iot status other than ok
          } else {

            // turn off programmable led indicating there is no active IOT link
            i2cComm.processCommand(I2C_SET_LED_OFF, data, i2cData);            
          }

          // set latest firmware version
          webserver.setFirmwareUpdateVersion(iotResponseJsonObject.get<unsigned int>("latestversion"));
            
          // new software version available?
          if (iotResponseJsonObject.get<unsigned int>("latestversion") > FIRMWARE_VERSION) {

            // start update
            webserver.doUpdate();
          }

        // json parse not succesful? might be a server error
        // turn off the led, indicating the IOT link has been lost
        } else {


          // code below is to make sure that updates are rolled out even if iot-functionality
          // is broken
          webserver.getFirmwareUpdateVersion();

          
          // turn off programmable led indicating there is no active IOT link
          i2cComm.processCommand(I2C_SET_LED_OFF, data, i2cData);   
        }
      }
    } 

    // increment the iteration counter
    iterations ++;
  }
}
