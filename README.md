Pond[CTRL] - Atmega/ESP8266 based pond controller

Uses an ESP8266 WiFi module and Atmega328 microcontroller communicating over I2C.
* ESP8266 I2C Master
* Atmega328 I2C Slave

Protocol
* ESP8266 sends 17 bytes (1 command byte, 16 data bytes)
* Atmega responds with 16 data bytes

ESP8266
Acts as an access-point until a WiFi connection is established with a router.

... more information will follow ...

Source code exists of:
* atmega328 firmware (uploadable using onboard) USB controller
* esp8266 firmware (can be upgraded OTA or with an ESP-programmer)

Written using the Arduino IDE. 

To compile/upload atmega firmware, use the IDE with arduino UNO settings. 

To compile esp8266 firmware, use following board manager url 
(both 32 and 64 bit Windows versions are supported):

https://pondctrl.dennisbor.com/source/arduino_boards/esp8266.json

Compile using the following board settings:

Board: Generic ESP8266 Module
Builtin Led: 2
Upload Speed: 115200
CPU Frequeny: 80MHz
Crystal Frequency: 26 MHz
Flash Size: 1MB (FS: none, OTA:~512KB)
Flash Mode: DOUT (compatible)
Flash Frequency (40 MHz)
Reset Method: dtr (aka nodemcu)
Debug port: disabled
Debug level: none
IwIP Variant: v2 Lower Mempry
VTables: Flash
Exceptions: Legacy (new can return nullptr)
Erase Flash: Only Sketch
Espressif FW: nonos-sdk 2.2.1+100 (190703)
SSL Support: All SSL ciphers (most compatible)

