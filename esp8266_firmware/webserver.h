/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */

#ifndef webserver_h
#define webserver_h

// external libraries
#include <ESP8266WebServer.h> 
#include "arduino.h"
#include <ArduinoJson.h>          // requires version 5.13.4
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

// own libraries
#include "sensor.h"
#include "socket.h"
#include "i2c.h"
#include "index.h"

/**
 * WebServer class
 */
class WebServer {

  private:
  
    // private variables

    // IPAddress to store ip from logged in user for
    // authentication
    IPAddress administrator;

    // I2C and MemoryTools objects
    I2C i2cComm;
    MemoryTools memTools;
    
    // integers to store current and target firmware versions
    int firmwareUpdateVersion = 0;
    int currentFirmwareVersion = 0;

    // default url prefix for updates
    // version url (unformatted string): <firmwareURLBase>.version
    // binary url: <firmwareURLBase>.<version>.bin
    const char* firmwareURLBase = "http://pondctrl.dennisbor.com/update/wifi/firmware";

    // char to store current wireless mode
    // this can be used to, for example, redirect to the set wifi page of
    // an interface if not connected to a router
    char module_wireless_mode = 0;

    // strings to store WiFi module ssid, passphrase and hostname and administrator credentials
    String module_ssid, module_passphrase, module_mdns_hostname, module_gui_username, module_gui_password;

    // string to display custom debug data
    String customData = "";

    // api (application programming interface) key for interaction between devices without
    // authorization
    String apiKey = "";

    // array of unsigned ints to store sensor values
    // sensor values are not taken from Pond[CTRL]™ directly, because of annoying fluctuation in
    // some cheap sensors. The sensor value is calculated based on average of the last ten readings
    // before sending to client.
    unsigned int sensorValues[5];

    // boolean to hold if data logging is active. data logging and alarm calculations are only started after five
    // minutes active time, to give sensors time to stabilize 
    bool active = false;

    // unsigned long to store timestamp from last feedpause activation
    unsigned long feedPauseActivated = 0;

    // unsigned long to store current uptime
    unsigned long uptime = 0;

    // unsigned long to store current unix epoch
    unsigned long timestamp = 0;   

    // unsigned int to store feedpause duration in seconds, default 1800 (half an hour)
    unsigned int feedpauseDuration = 1800;

    // int to store timezone offset from UTC in seconds, 3600 (1 hour) for Netherlands
    int timezoneOffset = 3600;

    // time server location, default is pool.ntp.org
    String timeserver = "pool.ntp.org";

    // upnp external port, default 7777
    unsigned int upnpPort = 7777;
    
    // boolean to hold if a feedpause is active
    bool feedPause = false;

    // boolean to hold if sensor alarm auto recovery is enabled
    bool alarmAutoRecovery = false;

    // boolean to hold if alarm buzzer is on
    bool alarmSoundOn = false;

    // private function declarations
    void outputJSON(String jsonOutput);
    void doLogin();
    void setWiFi();
    void listNetworks();
    void doLogout();
    void query();
    void setAdmin();
    void getMac();
    void showInterface();
    bool isAuthorized();
    void postProcessSensorValues(byte data[16]);
    void postProcessSensorSettings(unsigned int sensorIndex, byte data[16]);
    void toggleSocket(unsigned int socketIndex, bool toggleOn);
    void calculateSocketState(unsigned int socketIndex, unsigned int sensorIndex, int modeSet, int requiredState);
    void detectSocketWarnings();
    void toggleFeedPause();
    void toggleAlarmSound();
    void toggleAlarmAutoRecovery();
    void get();
    void setSensor();
    void setSocket();
    void setSystem();
    void setPHSensorCalibration();
    void setTemperatureSensorCalibration();
    void reset();
    
  public:

    // sensor and socket arrays
    Sensor sensors[5];
    Socket sockets[5];

    // public member functions
    void setWirelessMode(char mode);
    void setSSIDPassPhraseAndMDSNHostname(String targetSSID, String targetPassphrase, String mdnsHostname);
    void setOtherSettings(String apiKey, unsigned int alarmSound, unsigned int alarmAutoReset, unsigned int feedpauseDurationFromEEPROM, int timezomeOffsetFromEEPROM, String timeserverFromEEPROM, unsigned int upnpPortFromEEPROM);
    void setGUICredentials(String username, String password);
    void setCustomData(String data);
    void setFirmwareVersion(unsigned int versionNumber);
    void handleRequests();
    void setSensorValues();
    void activateSensorData();
    void calculateSensorState();
    void switchSockets();
    void forceSocketSwitching();
    void setFirmwareUpdateVersion(unsigned int versionNumber);
    void doUpdate();
    void checkFeedPause();
    void setApiKey();
    void setUptime(unsigned long timestamp);
    unsigned int getFirmwareUpdateVersion();
    void setTimestamp(unsigned long stamp);
    int getTimezoneOffset();
    String getTimeserver();
    unsigned int getUPnPPort();

    WebServer();
    
};


#endif
