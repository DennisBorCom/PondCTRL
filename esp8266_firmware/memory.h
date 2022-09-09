/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */

#ifndef memory_h
#define memory_h

// external libraries
#include "arduino.h"
#include <EEPROM.h>
#include <ESP8266WebServer.h> 

/**
 * MemoryTools class
 */
class MemoryTools {
  
  private:

    // private function declarations
    void clear();
  
  public:

    // public constants declarations - declarations are in {address, length in bytes}

    // network settings
    const word EEPROM_SSID[2]                               = {0, 32};        // last connected access point SSID
    const word EEPROM_PASSPHRASE[2]                         = {32, 63};       // last connected access point passphrase
    const word EEPROM_MDNSRESPONDER[2]                      = {238, 16};      // mdns responser prefix, default 'pondctrl' (results in pondctrl.local)
    const word EEPROM_UPNP_PORT[2]                          = {358, 2};       // external UPnP port, default '7777' 

    // user interface / remote credentials
    const word EEPROM_GUI_USERNAME[2]                       = {95, 16};       // username, default 'admin'
    const word EEPROM_GUI_PASSWORD[2]                       = {111, 32};      // password, default 'admin'
    const word EEPROM_APIKEY[2]                             = {256, 32};      // API-key, no default, must be generated 
    
    // timeserver settings            
    const word EEPROM_TIMESERVER_ADDRESS[2]                 = {294, 64};      // timeserver address, default 'pool.ntp.org'
    const word EEPROM_TIMESERVER_UTC_OFFSET[2]              = {290, 4};       // utc offset in seconds. first two bytes represent positive correction (max 14 hours * 3600 seconds)
                                                                              // second two bytes represent negative correction (max 12 hours * 3600 seconds)
    // socket settings
    const word EEPROM_SOCKETS_AFFECTEDBYMAINTENANCE[2]      = {360, 1};       // 1 combined byte to hold if sockets are affected by maintenance
    const word EEPROM_SOCKETS_AFFECTEDBYFEEDPAUSE[2]        = {363, 1};       // 1 combined byte to hold if sockets are affected by feedpause
    const word EEPROM_SOCKETS_MODE[2]                       = {364, 5};       // socket mode, stored per byte per socket
    const word EEPROM_SOCKETS_MAX_ON_TIME[2]                = {369, 10};      // socket maximum on time in seconds, stored per 2 bytes (max 65535) per socket
    const word EEPROM_SOCKETS_NAME[2]                       = {379, 160};     // socket name, max 32 characters
    const word EEPROM_SOCKETS_AGE[2]                        = {539, 20};      // socket device age, holds the number of seconds the socket is activated, 4 bytes (unsigned long) per socket
    
    // global settings
    const word EEPROM_ALARM_AUTORESET[2]                    = {254, 1};       // alarm autoreset, default is on (1)
    const word EEPROM_ALARM_SOUND[2]                        = {255, 1};       // alarm sound, default is off (0)
    const word EEPROM_FEEDPAUSE_DURATION[2]                 = {288, 2};       // feed pause duration in seconds, default is 300, max is 65535

    // localization settings
    const word EEPROM_GPS_LATITUDE[2]                       = {559, 4};       // gps latitude (float), no default
    const word EEPROM_GPS_LONGITUDE[2]                      = {563, 4};       // gps longitude (float), no default

    // Note: next available memory offset address is 567, total size is 4096
    // Note: 2 bytes free: 361, 362

    // public function declarations
    void hardReset();
    void write(const word parameter[2], String data);
    void write(unsigned int offset, unsigned int length, String data);
    void write(unsigned int offset, unsigned long value);
    void writeChar(unsigned int offset, char data);
    String read(const word parameter[2]);
    String read(unsigned int offset, unsigned int length);
    unsigned long readUnsignedLong(unsigned int offset);
    char read(unsigned int offset);
    void checkFirstRun();
    char getBitFromCombinedByte(unsigned int memoryOffset, unsigned int index);
    void setBitForCombinedByte(unsigned int memoryOffset, unsigned int index, char value);
    void writeFloat(unsigned int offset, float value);
    float readFloat(unsigned int offset);
};

#endif
