# Pond[CTRL] - The open source pond controller<br>
<br>
__PLEASE NOT THAT THIS INFORMATION IS NOT COMPLETE YET__<br>
<br>
An open source pond controller able to measure temperature, pH and level and switch AC sockets.<br>

Pond[CTRL]'s hardware is consists of a self-designed PCB with two major parts:
* an ATmega328 microcontroller handling measurements and socket switching
* an ESP8266 WiFi module controlling the ATmega  

__Communication__
For communication between the ATmega328 and ESP8266 the I2C bus is used, with a logic level converter (ATMega runs on 5V while the ESP8266 is unable to handle voltages above 3.3V). The ESP8266 acts as I2C master, sending 17 bytes to the ATmega328 slave: 1 command byte and 16 data bytes. The ATmega then responds with 16 bytes.

__Initial setup__
The ESP8266 has a very limited user interface, only capable of connecting to your WiFi-network. While not connected, it acts as an access point _'Pond[CTRL] by Dennis Bor'_ with a captive portal forwarding to the initial setup page and a mDNS responder. Go to [pondctrl.local](pondctrl.local) to access the device. Write down the device serial number when shown, because you need it to register your device once in the online user interface.

When connected to the internet, the device is registered and starts uploading device information and measurement data to the server. The status light will light up when succesfully connected. Pond[CTRL] uses UPnP to automatically add port forwarding to access your device from outside the network. 

__User interface__
The ESP8266 is very limited in storage and to be able to receive OTA updates, the user interface is hosted online instead of on the device itself. The user interface connects to your device, using information sent by itself to the servers. The user interface is accessible through [pondctrl.dennisbor.com](https://pondctrl.dennisbor.com). You need to create a user account and register your device once using the device serial number obtained in the initial setup. 

The Pond[CTRL] user interface will show:
* Actual measurement data for enabled sensors
* Actual socket states
* Alarm status
* Remaining feed pause duration (when active)
 
Configurable sensor settings:
* Sensor state (enabled/disabled)
* Sensor name
* Sensor lowest acceptable value
* Sensor highest acceptable value
* Sensor low alarm trigger value
* Sensor high alarm trigger value
* Sensor threshold
* Sensor correction (temperature sensors only)
* Sensor calibration (pH sensors only)

Configurable socket settings:
* Socket name
* If affected by feed pause (toggles off if feed pause is activated)
* Switch mode (sensor low/high activated or permanent state)
* Maximum on time before raising an alarm

Configurable WiFi settings:
* SSID selection
* mDNS responder name
* will fallback to previous settings if connection not succesful

Configurable Administration settings:
* user interface username and password
* web interface port number

Configurable system settings:
* feed pause duration in seconds
* timezone UTC offset (for daylight saving time purposes)0
* timeserver URL

API key settings:
* random api key generation (64 characters) for other apps to be able to control the device without username and password authentication.

__Button behaviour__
The device button has different functions, depending on the duration it is pressed:
* 0 - 4 seconds: activate/deactivate feed pause
* 5 - 14 seconds: Pond[CTRL] sensor and socket hard reset
* more than 14 seconds: Pond[CTRL] WiFi module hard reset

# Source code

Source code exists of:

* atmega328 firmware (uploadable using onboard USB controller)
* esp8266 firmware (initial firmware must bu uploaded using an ESP-programmer and can then be upgraded OTA)

The source code is written using the Arduino IDE. To compile/upload atmega firmware, use the IDE with arduino UNO settings. To compile ESP8266 firmware, use following board manager url (both 32 and 64 bit Windows versions are supported) [https://pondctrl.dennisbor.com/source/arduino_boards/esp8266.json](https://pondctrl.dennisbor.com/source/arduino_boards/esp8266.json)

Compile using the following board settings:

* Board: Generic ESP8266 Module
* Builtin Led: 2
* Upload Speed: 115200
* CPU Frequeny: 80MHz
* Crystal Frequency: 26 MHz
* Flash Size: 1MB (FS: none, OTA:~512KB)
* Flash Mode: DOUT (compatible)
* Flash Frequency (40 MHz)
* Reset Method: dtr (aka nodemcu)
* Debug port: disabled
* Debug level: none
* IwIP Variant: v2 Lower Mempry
* VTables: Flash
* Exceptions: Legacy (new can return nullptr)
* Erase Flash: Only Sketch
* Espressif FW: nonos-sdk 2.2.1+100 (190703)
* SSL Support: All SSL ciphers (most compatible)

The following libraries are required (Can be downloaded from the arduino_libraries folder):

* NTPClient v 3.2.0
* ArduinoJSON  5.13.4
* DallasTemperature v 3.8.0
* OneWire 2.3.5
* TinyUPnP 3.1.4 (customized version, supporting different internal and external port numbers)

ATmega Pinout:

GPIO2  /  2   :     Socket 1 switching
GPIO3  /  3   :    Socket 2 switching
GPIO4  /  4   :    Socket 3 switching
GPIO5  /  5   :    Button
GPIO6  /  6   :    Socket 4 switching
GPIO7  /  7   :    Socket 5 switching
GPIO9  /  9   :    OneWire temperature sensor #2
GPIO10 / 10   :    Status LED
GPIO11 / 11   :    Speaker
GPIO12 / 12   :    Level switch
GPIO13 / 13   :    OneWire temperature sensor #1
GPIO15 / A1   :    pH sensor #2
GPIO17 / A3   :    pH sensor #1
