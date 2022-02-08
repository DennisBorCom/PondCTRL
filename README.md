# Pond[CTRL] - The open source pond controller <br>
<br>
__PLEASE NOT THAT THIS INFORMATION IS NOT COMPLETE YET__ <br>
<br>
An open source pond controller able to measure temperature, pH and level and switch AC sockets.<br>
<br>
Pond[CTRL]'s hardware is consists of a self-designed PCB with two major parts:<br>
* an ATmega328 microcontroller handling measurements and socket switching<br>
* an ESP8266 WiFi module controlling the ATmega  <br>
<br>
__Communication__ <br>
For communication between the ATmega328 and ESP8266 the I2C bus is used, with a logic level converter (ATMega runs on 5V while the ESP8266 is unable to handle voltages above 3.3V). The ESP8266 acts as I2C master, sending 17 bytes to the ATmega328 slave: 1 command byte and 16 data bytes. The ATmega then responds with 16 bytes.<br>
<br>
__Initial setup__ <br>
The ESP8266 has a very limited user interface, only capable of connecting to your WiFi-network. While not connected, it acts as an access point _'Pond[CTRL] by Dennis Bor'_ with a captive portal forwarding to the initial setup page and a mDNS responder. Go to [pondctrl.local](pondctrl.local) to access the device. Write down the device serial number when shown, because you need it to register your device once in the online user interface.<br>
<br>
When connected to the internet, the device is registered and starts uploading device information and measurement data to the server. The status light will light up when succesfully connected. Pond[CTRL] uses UPnP to automatically add port forwarding to access your device from outside the network. <br>
<br>
__User interface__ <br>
The ESP8266 is very limited in storage and to be able to receive OTA updates, the user interface is hosted online instead of on the device itself. The user interface connects to your device, using information sent by itself to the servers. The user interface is accessible through [pondctrl.dennisbor.com](https://pondctrl.dennisbor.com). You need to create a user account and register your device once using the device serial number obtained in the initial setup. <br>
<br>
The Pond[CTRL] user interface will show:<br>
* Actual measurement data for enabled sensors<br>
* Actual socket states<br>
* Alarm status<br>
* Remaining feed pause duration (when active)<br>
<br> 
Configurable sensor settings:<br>
* Sensor state (enabled/disabled)<br>
* Sensor name<br>
* Sensor lowest acceptable value<br>
* Sensor highest acceptable value<br>
* Sensor low alarm trigger value<br>
* Sensor high alarm trigger value<br>
* Sensor threshold<br>
* Sensor correction (temperature sensors only)<br>
* Sensor calibration (pH sensors only)<br>
<br>
Configurable socket settings:<br>
* Socket name<br>
* If affected by feed pause (toggles off if feed pause is activated)<br>
* Switch mode (sensor low/high activated or permanent state)<br>
* Maximum on time before raising an alarm<br>
<br>
Configurable WiFi settings:<br>
* SSID selection<br>
* mDNS responder name<br>
* will fallback to previous settings if connection not succesful<br>
<br>
Configurable Administration settings:<br>
* user interface username and password<br>
* web interface port number<br>
<br>
Configurable system settings:<br>
* feed pause duration in seconds<br>
* timezone UTC offset (for daylight saving time purposes)<br>
* timeserver URL<br>
<br>
API key settings:<br>
* random api key generation (64 characters) for other apps to be able to control the device without username and password authentication.<br>
<br>
__Button behaviour__ <br>
The device button has different functions, depending on the duration it is pressed:<br>
* 0 - 4 seconds: activate/deactivate feed pause<br>
* 5 - 14 seconds: Pond[CTRL] sensor and socket hard reset<br>
* more than 14 seconds: Pond[CTRL] WiFi module hard reset<br>
<br>
# Source code <br>
<br>
Source code exists of:<br>
<br>
* atmega328 firmware (uploadable using onboard USB controller)<br>
* esp8266 firmware (initial firmware must bu uploaded using an ESP-programmer and can then be upgraded OTA)<br>
<br>
The source code is written using the Arduino IDE. To compile/upload atmega firmware, use the IDE with arduino UNO settings. To compile ESP8266 firmware, use following board manager url (both 32 and 64 bit Windows versions are supported) [https://pondctrl.dennisbor.com/source/arduino_boards/esp8266.json](https://pondctrl.dennisbor.com/source/arduino_boards/esp8266.json)<br>
<br>
Compile using the following board settings:<br>
<br>
* Board: Generic ESP8266 Module<br>
* Builtin Led: 2<br>
* Upload Speed: 115200<br>
* CPU Frequeny: 80MHz<br>
* Crystal Frequency: 26 MHz<br>
* Flash Size: 1MB (FS: none, OTA:~512KB)<br>
* Flash Mode: DOUT (compatible)<br><br>
* Flash Frequency (40 MHz)<br>
* Reset Method: dtr (aka nodemcu)<br>
* Debug port: disabled<br>
* Debug level: none<br>
* IwIP Variant: v2 Lower Mempry<br>
* VTables: Flash<br>
* Exceptions: Legacy (new can return nullptr)<br>
* Erase Flash: Only Sketch<br>
* Espressif FW: nonos-sdk 2.2.1+100 (190703)<br>
* SSL Support: All SSL ciphers (most compatible)<br>
<br>
The following libraries are required (Can be downloaded from the arduino_libraries folder):<br>
<br>
* NTPClient v 3.2.0<br>
* ArduinoJSON  5.13.4<br>
* DallasTemperature v 3.8.0<br>
* OneWire v 2.3.5<br>
* TinyUPnP v 3.1.4 (customized version, supporting different internal and external port numbers)<br>
<br>
ATmega Pinout:<br>
<br>
GPIO2  /  2   :    Socket 1 switching<br>
GPIO3  /  3   :    Socket 2 switching<br>
GPIO4  /  4   :    Socket 3 switching<br>
GPIO5  /  5   :    Button<br>
GPIO6  /  6   :    Socket 4 switching<br>
GPIO7  /  7   :    Socket 5 switching<br>
GPIO9  /  9   :    OneWire temperature sensor #2<br>
GPIO10 / 10   :    Status LED<br>
GPIO11 / 11   :    Speaker<br>
GPIO12 / 12   :    Level switch<br>
GPIO13 / 13   :    OneWire temperature sensor #1<br>
GPIO15 / A1   :    pH sensor #2<br>
GPIO17 / A3   :    pH sensor #1<br>
