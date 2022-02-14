/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */

#include "memory.h"

/**
 * Writes a char to the EEPROM memory
 * 
 * @param           offset            memory offset
 * @param           data              char to write
 * 
 * @return          this function does not produce a return value
 */
void MemoryTools::writeCharToMemory(unsigned int offset, char data) {
  
  // write char to memory
  EEPROM.write(offset, data);
  EEPROM.commit();
}

/**
 * Reads a char from the EEPROM memory
 * 
 * @param           offset            memory offset
 * 
 * @return          returns the char at offset
 */
char MemoryTools::readCharFromMemory(unsigned int offset) {

  // read char and return
  return EEPROM.read(offset);
}

/**
 * Writes data to the EEPROM memory 
 * 
 * @param           parameter          array with mempry offset and length  
 * @param           data               data to write
 * 
 * @return          this function does not produce a return value
 */
void MemoryTools::writeMemory(const word parameter[2], String data) {

  // iterate the number of times specified in the parameter array (second value, index 1)
  for (int i = 0; i < parameter[1]; ++i) {
    
    // data available at this position?
    if (i < data.length()) {
      
      // yes, write the current character to the EEPROM at the calculated offset
      EEPROM.write(parameter[0] + i, data[i]);

    } else {

      // no, write zero value
      EEPROM.write(parameter[0] + i, 0);
    }
  }

  // commit changes to EEPROM
  EEPROM.commit();
}

/**
 * Writes data to the EEPROM memory
 * 
 * @return          this function does not produce a return value
 */
void MemoryTools::checkFirstRun() {

  // check if the first byte of the EEPROM is 255. 
  // values are stored from the very first byte in the EEPROM memory
  // so we can detect a first run by checking the first bytel if 255
  // we can hard reset the module (write the default values)

  // first byte equals 255
  if (EEPROM.read(0) == 255) {

    Serial.println("Flash reset");

    // perform a hard reset (default values will be stored in memory)
    hardReset();
  }
}

/**
 * Writes data to the EEPROM memory
 * 
 * @param           parameter               array with mempry offset and length 
 * 
 * @return          the string, read from memory
 */
String MemoryTools::readMemory(const word parameter[2]) {

  // empty string to store the value in
  String value;

  // iterate the number of times specified in the parameter array (second value, index 1)
  for (int i = parameter[0]; i < (parameter[0] + parameter[1]); ++i) {
    
    // byte value at current position higher than zero?
    if (EEPROM.read(i) > 0) {

       // append value to string
       value += char(EEPROM.read(i));
    }
  }

  // return the string
  return value;
}


/**
 * Performs a hard reset (write defaults to memory)
 * 
 * @return          this function does not produce a return value
 */
void MemoryTools::hardReset() {

  // clear mempry
  clearMemory();

  // write defaults to memory:
  // - access point ssid and passphrase empty
  // - gui username and password 'admin'
  // - mdns responder to pondctrl (resulting in pondctrl.local)    
  writeMemory(EEPROM_SSID, "");
  writeMemory(EEPROM_PASSPHRASE, "");
  writeMemory(EEPROM_GUI_USERNAME, "admin");
  writeMemory(EEPROM_GUI_PASSWORD, "admin");
  writeMemory(EEPROM_MDNSRESPONDER, "pondctrl");
  writeCharToMemory(EEPROM_UPNP_PORT[0], 7777 >> 8);
  writeCharToMemory(EEPROM_UPNP_PORT[0] + 1, 7777 & 255);
  writeCharToMemory(EEPROM_FEEDPAUSE_DURATION[0], 300 >> 8);
  writeCharToMemory(EEPROM_FEEDPAUSE_DURATION[0] + 1, 300 & 255);
  writeCharToMemory(EEPROM_ALARM_AUTORESET[0], 1);
  writeCharToMemory(EEPROM_ALARM_SOUND[0], 0);
  writeMemory(EEPROM_TIMESERVER_ADDRESS, "ntp.pool.org");
  
  // disconnect active connections
  WiFi.disconnect();

  // restart the WiFi module 
  ESP.restart();
}

/**
 * Clears the memory
 * 
 * @return          this function does not produce a return value
 */
void MemoryTools::clearMemory() {

  // loop through all EEPROM bytes (4096)
  for (int i = 0; i < 4096; ++i)
  {
    // write zero byte at current offset 
    EEPROM.write(i, 0);
  }

  // commit changes
  EEPROM.commit();
}
