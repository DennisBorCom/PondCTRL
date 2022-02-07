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
    void clearMemory();
  
  public:

    // public constants declarations
    const word EEPROM_SSID[2] = {0, 32};
    const word EEPROM_PASSPHRASE[2] = {32, 63};
    const word EEPROM_GUI_USERNAME[2] = {95, 16};
    const word EEPROM_GUI_PASSWORD[2] = {111, 32};
    const word EEPROM_AP_PASSPHRASE[2] = {175, 63};
    const word EEPROM_MDNSRESPONDER[2] = {238, 16};
    const word EEPROM_ALARM_AUTORESET[2] = {254, 1};
    const word EEPROM_ALARM_SOUND[2] = {255, 1};
    const word EEPROM_APIKEY[2] = {256, 32};
    const word EEPROM_FEEDPAUSE_DURATION[2] = {288, 2};     // feed pause duration in seconds, max 65535
    const word EEPROM_TIMESERVER_UTC_OFFSET[2] = {290, 4};  // utc offset in seconds, first two bytes represent positive correction (max 14 hours * 3600 seconds)
                                                            // second two bytes represent negative correction (max 12 hours * 3600 seconds)
    const word EEPROM_TIMESERVER_ADDRESS[2] = {294, 64};    // timeserver address, default is pool.ntp.org
    const word EEPROM_UPNP_PORT[2] = { 358, 2};             // externe upnp poort (0-65535) 
    
    // public function declarations
    void hardReset();
    void writeMemory(const word parameter[2], String data);
    String readMemory(const word parameter[2]);
    void checkFirstRun();
    void writeCharToMemory(unsigned int offset, char data);
    char readCharFromMemory(unsigned int offset);
};

#endif
