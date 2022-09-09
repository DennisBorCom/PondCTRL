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
void MemoryTools::writeChar(unsigned int offset, char data) {
  
  // write char to memory
  EEPROM.write(offset, data);
  EEPROM.commit();
}

/**
 * Write a float to the EEPROM memory. Please note that a float uses 4 bytes in memory
 * 
 * @param           offset            memory offset
 * @param           value             floating point value to write
 * 
 * @return          this function does not produce a return value
 */
void MemoryTools::writeFloat(unsigned int offset, float value) {

  // write float to memory
  EEPROM.put(offset, value);
}

/**
 * Read a float from EEPROM memory. 
 * 
 * @param          offset             memory offset
 * 
 * @return         floating point value read from MEMORY
 */
float MemoryTools::readFloat(unsigned int offset) {

  // float to hold return value;
  float returnValue;

  // read float from offset
  EEPROM.get(offset, returnValue);
  
  // return floating point value read from memory
  return returnValue;
}

/**
 * Reads a char from the EEPROM memory
 * 
 * @param           offset            memory offset
 * 
 * @return          returns the char at offset
 */
char MemoryTools::read(unsigned int offset) {

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
void MemoryTools::write(const word parameter[2], String data) {

  // write to memory
  write(parameter[0], parameter[1], data);
}


/**
 * Writes data to the EEPROM memory 
 * 
 * @param           offset            EEPROM address offset  
 * @param           length            length to write
 * @param           data              data to write
 * 
 * @return          this function does not produce a return value
 */
void MemoryTools::write(unsigned int offset, unsigned int length, String data) {
  
  // iterate the number of times specified in the parameter array (second value, index 1)
  for (int i = 0; i < length; ++i) {
    
    // data available at this position?
    if (i < data.length()) {
      
      // yes, write the current character to the EEPROM at the calculated offset
      EEPROM.write(offset + i, data[i]);

    } else {

      // no, write zero value
      EEPROM.write(offset + i, 0);
    }
  }

  // commit changes to EEPROM
  EEPROM.commit();
}

/**
 * Returns the bit value from a combined byte (used for multiple settings stored in one single byte)
 * 
 * @return          bit value
 */
char MemoryTools::getBitFromCombinedByte(unsigned int memoryOffset, unsigned int index) {

  // read byte from memory
  char memoryByte = read(memoryOffset);

  // bitshift index right
  memoryByte = memoryByte >> index;
  
  // return bit value
  return memoryByte &= 1;
}

/**
 * Sets a bit in a combined byte (used for multiple settings stored in one single byte)
 */
void MemoryTools::setBitForCombinedByte(unsigned int memoryOffset, unsigned int index, char value) {

  // read byte from memory
  char memoryByte = read(memoryOffset);

  // bit 1 (true)
  if (value) {

    // bitwise OR the value 
    memoryByte |= (1 << index);
    
  } else {

    // distract the bit value from 255
    byte andByte = 255 - (1 << index);

    // bitwise AND the value
    memoryByte &= andByte;
  }

  // write back the value to memory
  writeChar(memoryOffset, memoryByte);
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
 * @param           memoryOffset               EEPROM offset address
 * @param           length                     length to read
 * 
 * @return          the string, read from memory
 */
String MemoryTools::read(unsigned int offset, unsigned int length) {
  
  // empty string to store the value in
  String value;

  // iterate the number of times specified in the parameter array (second value, index 1)
  for (int i = offset; i < (offset + length); ++i) {
    
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
 * Writes data to the EEPROM memory
 * 
 * @param           parameter               array with mempry offset and length 
 * 
 * @return          the string, read from memory
 */
String MemoryTools::read(const word parameter[2]) {

  // read from memory
  return read(parameter[0], parameter[1]);
  
}


/**
 * Writes an unsigned long value to EEPROM memory
 * 
 * @param           offset                offset in memory to write to
 * @param           value                 value to write
 * 
 * @return          this function does not produce a return value
 */
void MemoryTools::write(unsigned int offset, unsigned long value) {

  // write unsigned long to four bytes
  writeChar(offset, ((value >> 24) & 255));
  writeChar(offset + 1, ((value >> 16) & 255));
  writeChar(offset + 2, ((value >> 8) & 255));
  writeChar(offset + 3, (value & 255));
}

/**
 * Reads an unsigned long value from memory
 * 
 * @param           offset                offset in memory to read from
 * 
 * @return          unsigned long value
 */
unsigned long MemoryTools::readUnsignedLong(unsigned int offset) {

    // return unsigned long value
    return (read(offset) << 24) + (read(offset + 1) << 16) + (read(offset + 2) << 8) + read(offset + 3);
}

/**
 * Performs a hard reset (write defaults to memory)
 * 
 * @return          this function does not produce a return value
 */
void MemoryTools::hardReset() {

  // clear mempry
  clear();

  // write defaults to memory: 
  write(EEPROM_SSID, "");                                               // last known access point SSID will be blank
  write(EEPROM_PASSPHRASE, "");                                         // last known access point password will be blank
  write(EEPROM_GUI_USERNAME, "admin");                                  // default GUI username is 'admin'
  write(EEPROM_GUI_PASSWORD, "admin");                                  // default GUI password is 'admin'
  write(EEPROM_MDNSRESPONDER, "pondctrl");                              // default mDNS responder is 'pondctrl' (pondctrl.local)
  writeChar(EEPROM_UPNP_PORT[0], 7777 >> 8);                            // default upnp port is 7777 (needs 2 bytes for storage)
  writeChar(EEPROM_UPNP_PORT[0] + 1, 7777 & 255);
  writeChar(EEPROM_FEEDPAUSE_DURATION[0], 300 >> 8);                    // feed pause default duration is 300 seconds (needs 2 bytes for storage)
  writeChar(EEPROM_FEEDPAUSE_DURATION[0] + 1, 300 & 255);
  writeChar(EEPROM_ALARM_AUTORESET[0], 1);                              // alarm autoreset enabled by default
  writeChar(EEPROM_ALARM_SOUND[0], 0);                                  // alarm sound disabled by default
  writeChar(EEPROM_SOCKETS_AFFECTEDBYMAINTENANCE[0], 0);                // no sockets are affected by maintenance by defeault
  write(EEPROM_TIMESERVER_ADDRESS, "ntp.pool.org");                     // default time server address 'ntp.pool.org'
  write(EEPROM_SOCKETS_NAME[0], 32, "Socket #1");                       // default socket names (Socket #1 - Socket #5)
  write(EEPROM_SOCKETS_NAME[0] + 32, 32, "Socket #2");
  write(EEPROM_SOCKETS_NAME[0] + 64, 32, "Socket #3");
  write(EEPROM_SOCKETS_NAME[0] + 96, 32, "Socket #4");
  write(EEPROM_SOCKETS_NAME[0] + 128, 32, "Socket #5");   
  write(EEPROM_SOCKETS_AGE[0], (unsigned long)0);                       // socket age (seconds activated)
  write(EEPROM_SOCKETS_AGE[0] + 4, (unsigned long)0);
  write(EEPROM_SOCKETS_AGE[0] + 8, (unsigned long)0);
  write(EEPROM_SOCKETS_AGE[0] + 12, (unsigned long)0);
  write(EEPROM_SOCKETS_AGE[0] + 16, (unsigned long)0);
  
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
void MemoryTools::clear() {

  // loop through all EEPROM bytes (4096)
  for (int i = 0; i < 4096; ++i)
  {
    // write zero byte at current offset 
    EEPROM.write(i, 0);
  }

  // commit changes
  EEPROM.commit();
}
