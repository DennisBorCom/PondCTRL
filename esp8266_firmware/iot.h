/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */

#ifndef iot_h
#define iot_h

// external libraries
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

// own libraries
#include "sensor.h"
#include "socket.h" 

/**
 * IOTLink class
 */
class IOTLink {
  
    private:

    public:

        // public function declarations
        String processData(Sensor sensors[5], Socket sockets[5], unsigned int firmwareVersion, unsigned long currentTimestamp, unsigned int externalPort, float latitude, float longitude);
};

#endif
