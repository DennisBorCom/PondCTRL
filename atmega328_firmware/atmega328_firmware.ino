
/**
   Pond[CTRL] by Dennis Bor - info@dennisbor.com - www.dennisbor.com

   Firmware for PCB version 3.x

   Sensor-order:
   1. pH1
   2. pH2
   3. Temp1
   4. Temp2
   5. Level

   TODO: 

   * explain the adc readings 285/475
   * 2019-12-13 - sensor low high alarm
   * 2019-12-14 - sensor state changes using threshold
   * i2c protocol implementation
   * 2019-12-14 - feed pause (socket.affectedByFeedPause)
   * 2019-12-13 - maximum ontime (socket.maxOnTime)-->2019-12-13
   *  +- 2019-12-13 *FIXED* bug: when raising alarm, socket should turn of, alarm remain
   * button implementation (operations by time pressed)-->2019-12-13
   *  +- todo: implementing feed pause and wifi reset scripts
   * socket override
   * 2019-12-13 - socket state for ALWAYS_ON and ALWAYS_OFF

  IMPORTANT THINGS, LEARNED SO FAR:

   * I2C communication will fail if combined with serial output
   * I2C communication will fail if combined with typecasting
*/

#include "button.h"
#include "convert.h"          // conversion functions
#include "DallasTemperature.h"
#include "hardware.h"         // general hardware function and definitions
#include "i2c.h"              // i2c protocol / esp8266 communication functions
#include "memory.h"
#include "OneWire.h"
#include "sensor.h"           // sensor type definitions / functions
#include "socket.h"           // socket switching functions
#include "Wire.h"             // use wire library for i2c communication between ATMEGA and ESP

// initialize 5 new socket objects
Socket sockets[5];

// initialize an array with 5 sensors
Sensor sensors[5];

// initialize two temp sensors
OneWire tempwire1(PIN_TEMP1), tempwire2(PIN_TEMP2);
DallasTemperature temp1(&tempwire1), temp2(&tempwire2);

// byte array to hold bytes received from wifi module
// and single byte to hold wifi special command
byte bytesFromWiFiModule[17] = {};
byte specialCommandToWiFiModule = 0;

// integer to hold number of seconds since boot
unsigned long lastLoopTime;

// boolean to hold if feedpause is active
bool feedPause = false;
unsigned long feedPauseActivated = 0;

bool storeSettings = false;
bool i2cBusy = false;

void readSettings() {
  // read all settings

  // byte array for socket/sensor name storage
  byte name[16];
  
  // read pH1 sensor name
  readBytesFromMemory(EEPROM_PH1_NAME, name, 16);


  // set pH1 sensor low and high values and threshold
  setSensorValues(
    sensors[0], 
    readUnsignedIntFromMemory(EEPROM_PH1_LOW),
    readUnsignedIntFromMemory(EEPROM_PH1_HIGH),
    readUnsignedIntFromMemory(EEPROM_PH1_THRESHOLD),
    name,
    readUnsignedIntFromMemory(EEPROM_PH1_ALARM_LOW),
    readUnsignedIntFromMemory(EEPROM_PH1_ALARM_HIGH),
    0,
    readUnsignedIntFromMemory(EEPROM_PH1_4_CALIBRATION),
    readUnsignedIntFromMemory(EEPROM_PH1_7_CALIBRATION)
  );
  
  // read pH2 sensor name
  readBytesFromMemory(EEPROM_PH2_NAME, name, 16);
  
  // set pH2 sensor low and high values and threshold
  setSensorValues(
    sensors[1],
    readUnsignedIntFromMemory(EEPROM_PH2_LOW),
    readUnsignedIntFromMemory(EEPROM_PH2_HIGH),
    readUnsignedIntFromMemory(EEPROM_PH2_THRESHOLD),
    name,
    readUnsignedIntFromMemory(EEPROM_PH2_ALARM_LOW),
    readUnsignedIntFromMemory(EEPROM_PH2_ALARM_HIGH),
    0,
    readUnsignedIntFromMemory(EEPROM_PH2_4_CALIBRATION),
    readUnsignedIntFromMemory(EEPROM_PH2_7_CALIBRATION)
  );

  // read temperature sensor 1 name
  readBytesFromMemory(EEPROM_TEMP1_NAME, name, 16);
  
  // set temperature sensor 1 low and high and threshold values
  setSensorValues(
    sensors[2],
    readUnsignedIntFromMemory(EEPROM_TEMP1_LOW),
    readUnsignedIntFromMemory(EEPROM_TEMP1_HIGH),
    readUnsignedIntFromMemory(EEPROM_TEMP1_THRESHOLD),
    name,
    readUnsignedIntFromMemory(EEPROM_TEMP1_ALARM_LOW),
    readUnsignedIntFromMemory(EEPROM_TEMP1_ALARM_HIGH),
    readUnsignedIntFromMemory(EEPROM_TEMP1_CALIBRATION_POSITIVE) - readUnsignedIntFromMemory(EEPROM_TEMP1_CALIBRATION_NEGATIVE),
    0,
    0
  );

  //Serial.print(F("Read from ee (c1):"));
  //Serial.println(sensors[2].correctionValue);

  // read temperature sensor 2 name
  readBytesFromMemory(EEPROM_TEMP2_NAME, name, 16);
  
  // set temperature sensor 2 low and high and threshold values
  setSensorValues(
    sensors[3],
    readUnsignedIntFromMemory(EEPROM_TEMP2_LOW),
    readUnsignedIntFromMemory(EEPROM_TEMP2_HIGH),
    readUnsignedIntFromMemory(EEPROM_TEMP2_THRESHOLD),
    name,
    readUnsignedIntFromMemory(EEPROM_TEMP2_ALARM_LOW),
    readUnsignedIntFromMemory(EEPROM_TEMP2_ALARM_HIGH),
    readUnsignedIntFromMemory(EEPROM_TEMP2_CALIBRATION_POSITIVE) - readUnsignedIntFromMemory(EEPROM_TEMP2_CALIBRATION_NEGATIVE),
    0,
    0
  );

//Serial.print(F("Read from ee (c2):"));
  //Serial.println(sensors[3].correctionValue);

  // level switch name
  readBytesFromMemory(EEPROM_LEVEL_NAME, name, 16);
  
  // set level switch low and high and threshold values
  // note that a level switch only has two possible values: 0 (low) and 1 (high), and no threshold
  setSensorValues(
    sensors[4],
    0,
    1,
    0,
    name,
    readUnsignedIntFromMemory(EEPROM_LEVEL_ALARM_LOW),
    readUnsignedIntFromMemory(EEPROM_LEVEL_ALARM_HIGH),
    0,
    0,
    0
  );

  // set sensor 4 (level switch) to only on/off (without values)
  sensors[4].onOffOnly = true;

  // set socket settings (hardware pins, operation, if affected by feed pause and max on time)


  // socket 1 name
  readBytesFromMemory(EEPROM_SOCKET1_NAME, name, 16);
  
  // socket 1
  setSocketSettings(
    sockets[0], 
    PIN_SOCKET1, 
    readByteFromMemory(EEPROM_SOCKET1_OPERATION), 
    readByteFromMemory(EEPROM_SOCKET1_AFFECTEDBYFEEDPAUSE), 
    readUnsignedIntFromMemory(EEPROM_SOCKET1_MAXONTIME),
    name
  );

  // socket 2 name
  readBytesFromMemory(EEPROM_SOCKET2_NAME, name, 16);
  
  // socket 2
  setSocketSettings(
    sockets[1], 
    PIN_SOCKET2, 
    readByteFromMemory(EEPROM_SOCKET2_OPERATION), 
    readByteFromMemory(EEPROM_SOCKET2_AFFECTEDBYFEEDPAUSE), 
    readUnsignedIntFromMemory(EEPROM_SOCKET2_MAXONTIME),
    name
  );

  // socket 3 name
  readBytesFromMemory(EEPROM_SOCKET3_NAME, name, 16);
  
  // socket 3
  setSocketSettings(
    sockets[2], 
    PIN_SOCKET3, 
    readByteFromMemory(EEPROM_SOCKET3_OPERATION), 
    readByteFromMemory(EEPROM_SOCKET3_AFFECTEDBYFEEDPAUSE), 
    readUnsignedIntFromMemory(EEPROM_SOCKET3_MAXONTIME),
    name
  );

  // socket 4 name
  readBytesFromMemory(EEPROM_SOCKET4_NAME, name, 16);
  
  // socket 4
  setSocketSettings(
    sockets[3], 
    PIN_SOCKET4, 
    readByteFromMemory(EEPROM_SOCKET4_OPERATION), 
    readByteFromMemory(EEPROM_SOCKET4_AFFECTEDBYFEEDPAUSE), 
    readUnsignedIntFromMemory(EEPROM_SOCKET4_MAXONTIME),
    name
  );

  // socket5 name
  readBytesFromMemory(EEPROM_SOCKET5_NAME, name, 16);
  
  // socket 5
  setSocketSettings(
    sockets[4], 
    PIN_SOCKET5, 
    readByteFromMemory(EEPROM_SOCKET5_OPERATION), 
    readByteFromMemory(EEPROM_SOCKET5_AFFECTEDBYFEEDPAUSE), 
    readUnsignedIntFromMemory(EEPROM_SOCKET5_MAXONTIME),
    name
  );

  // read alarm mute state
 // alarmMute = readByteFromMemory(EEPROM_ALARM_MUTE);

  //Serial.println(alarmMute);

  // read alarm autoreset state
 // alarmAutoReset = readByteFromMemory(EEPROM_ALARM_AUTORESET);


  // enable/disbale sensors
  for (char sensorIterator = 0; sensorIterator < 5; sensorIterator ++) {

     // enable or disable the current sensor
     if (readByteFromMemory(sensorIterator) == 1) {

      sensors[sensorIterator].enabled = true;
      
      } else {
     
     sensors[sensorIterator].enabled = false;

      }

    // Serial.print(F("["));
     //Serial.print(byteArrayToString(sensors[sensorIterator].name, 16));
     //Serial.print(F("] "));
    // sensors[sensorIterator].enabled ? Serial.println(F("Enabled")) : Serial.println(F("Disabled"));
  }
}

void setup() {

  //hardReset();
  
  // initialize microprocessor hardware
  initializeHardware();

  // start serial connection with 115200 baud rate
  Serial.begin(115200);

  // i2c slave, address 32
  Wire.begin(32);
  
  // register i2c events
  Wire.onRequest(handleI2CRequest);
  Wire.onReceive(processI2CData);

  // read settings
  readSettings();

  // read temperature sensors before sensor loop
  // to prevent false alarms
  temp1.requestTemperatures();
  temp2.requestTemperatures();

  // delay to make sure sensors are readed properly
  delay(1000);
}




void loop() {


   lastLoopTime = millis() / 1000;
 
    
    
    
        // float to store sensor readings
        float floatValue;
      
        // iterate through all sensors 
        for (char sensorIterator = 0; sensorIterator < 5; sensorIterator++) {
      
      
          // if current sensor is enabled...
          if (sensors[sensorIterator].enabled == true) {
      
            // switch statement for sensor iterator (equals to the sensor number)
            switch (sensorIterator) {
      
              // pH sensor # 1
              case SENSOR_PH1: {
      
                // get floating point value for pin reading, mapped from calibration data    
                floatValue = mapf(analogRead(PIN_PH1), sensors[sensorIterator].adcPH4, sensors[sensorIterator].adcPH7, 4, 7);

                 if ((floatValue < 0) || (floatValue > 14)) {
                  floatValue = 0;
                 }
        
              } break;
      
              // ph sensor # 2
      
              case SENSOR_PH2: {
      
                // get floating point value for pin reading, mapped from calibration data    
                floatValue = mapf(analogRead(PIN_PH2), sensors[sensorIterator].adcPH4, sensors[sensorIterator].adcPH7, 4, 7);

                if ((floatValue < 0) || (floatValue > 14)) {
                  floatValue = 0;
                 }
                 
              } break;
      
              // temperature sensor # 1
              
              case SENSOR_TEMP1: {
      
                // get reading from sensor
                temp1.requestTemperatures();

                if (temp1.getTempCByIndex(0) >= 0) {
                  // get temperature in degrees celcius 
                  floatValue = temp1.getTempCByIndex(0);
                } else {
                  floatValue = 100 - temp1.getTempCByIndex(0);
                }
      
                
                         
              } break;
      
              // temperature sensor # 2
      
              case SENSOR_TEMP2: {
      
                // get reading from sensor
                temp2.requestTemperatures();
      
                if (temp2.getTempCByIndex(0) >= 0) {
                  // get temperature in degrees celcius 
                  floatValue = temp2.getTempCByIndex(0);
                } else {
                  floatValue = 100 - temp2.getTempCByIndex(0);
                }
         
              } break;
      
              // level switch
      
              case SENSOR_LEVEL: {
                
                 // get level switch state (inverted)
                 floatValue = !digitalRead(PIN_LEVEL);
                
              } break;
            }

           
        
  

             sensors[sensorIterator].value = (floatValue * 100) + sensors[sensorIterator].correctionValue;

             // print current sensor name and value to serial
          // Serial.print(F("["));
         //   Serial.print(byteArrayToString(sensors[sensorIterator].name, 16));
         //   Serial.print(F("] "));
         //  Serial.println(sensors[sensorIterator].value);

          //  Serial.print("DEBUG COR:");
           // Serial.println((int) sensors[sensorIterator].correctionValue);


            // calculate sensor state  
            //calculateSensorState(sensors[sensorIterator]);
              
          }

          
                   
        }


  // switch sockets according to sensor readings
  //switchSockets(sensors, sockets, feedPause);

  /*
  // feed pause activated
  if (feedPause && (feedPauseActivated == 0)) {
    feedPauseActivated = lastLoopTime;
  // feed pause running for more than an hour
  } else if (feedPause && ((lastLoopTime - feedPauseActivated) > 3600)) {
    for (unsigned int iterator = 0; iterator < 5; iterator ++) {
        sensors[iterator].stateChanged = SENSORSTATE_CHANGED;
    }
    feedPause = false;
    feedPauseActivated = 0;
  } else if (feedPause) {
  //  Serial.print(F("[feed pause] active for "));
   // Serial.print(lastLoopTime - feedPauseActivated);
   // Serial.println(F(" seconds"));
  } else {
    feedPauseActivated = 0;
  }
*/

  // alarm not muted?
  //if (!alarmMute) {
  
    // handle alarms according to sensor/socket state
    //handleAlarms(sensors, sockets, alarmMute);

  //}

  // handle button presses
  // if return true...
  if (handleButton(sensors, sockets, feedPause, specialCommandToWiFiModule)) {

    // ... reload settings
    readSettings();  
  }


  do {

      // loop
      delay(25);
      
  } while ((millis() / 1000) == lastLoopTime);
  
  if ((storeSettings == true) && (!i2cBusy)) {
      storeSettings = false;

      updateEEPROM(sensors, sockets);
  //    switchSockets(sensors, sockets, feedPause, alarmAutoReset);
  }

  
}


void emptyI2CDataArray() {
  for (unsigned int iterator = 0; iterator < 17; iterator ++ ) {
    bytesFromWiFiModule[iterator] = 0;
  }
}

void processI2CData(int numberOfBytes) {

 // Serial.println("[wifi module] request received");

  i2cBusy = true;
  
  emptyI2CDataArray();

  for (unsigned int iterator = 0; iterator < numberOfBytes; iterator ++ ) {
    bytesFromWiFiModule[iterator] = Wire.read();
  }

  

  /*for (unsigned int iterator = 0; iterator < numberOfBytes; iterator ++ ) {
    Serial.print("  ");
    Serial.print(bytesFromWiFiModule[iterator]);
    
  }

  Serial.println();*/
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void handleI2CRequest() {


  //hardReset();

  bool reloadSettings = false;

  byte dataToWiFiModule[16];

  
 
  if (getI2CResponse(sensors, sockets, bytesFromWiFiModule, reloadSettings, dataToWiFiModule, specialCommandToWiFiModule, feedPause) == true) {

  // for (unsigned int it = 0; it < 5; it ++) {
  //    Serial.println(sensors[it].alarmLow);
  //  }
   // Serial.println(alarmAutoReset);

    
    storeSettings = true;
    //readSettings();


    // hier moeten we dus settings gaan opslaan 
  }


  for (unsigned int i = 0; i < 16; i ++) {

       Wire.write(dataToWiFiModule[i]);

      
     
  
  }

  i2cBusy = false;
  

}
