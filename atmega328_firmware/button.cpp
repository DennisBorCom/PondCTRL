/**
 * Multifunctional button. 
 * 
 * Actions according to button press duration:
 * 
 * If an alarm is raised and the buzzer is on, the button will mute the buzzer.
 * 
 *   1 - 4 seconds: feed pause toggle on/off 
 *   5 - 14 seconds: hard reset PondCTRL
 *   >= 15 seconds: hard reset WiFi Module
 *
 */

#include "button.h"
#include "memory.h"

unsigned int buttonPressedTimestamp = 0;

/**
 * Beeps a specific time. Please not that this function will restore the previous state of the speaker so that in
 * case of an alarm, the speaker won't turn off.
 * 
 * @param           timeToBeepInMilliseconds    the time to beep
 */
void soundBeep(unsigned int timeToBeepInMilliseconds) {
  
  // save current state so that in case of alarm, the speaker won't be turned off
  unsigned int previousState = digitalRead(PIN_SPEAKER);

  // set speaker pin high (turn on)
  digitalWrite(PIN_SPEAKER, HIGH);

  // wait the specified time
  delay(timeToBeepInMilliseconds);

  // restore the previous state of the speaker pin
  digitalWrite(PIN_SPEAKER, previousState);
}

// return true = reload settings
bool handleButton(Sensor sensors[5], Socket sockets[5], bool &feedPause, byte &specialCommandToWiFiModule) {


    bool alarmRaised = false;

    for (unsigned int iterator = 0; iterator < 5; iterator ++) {
      alarmRaised |= sensors[iterator].alarmRaised;
      alarmRaised |= sockets[iterator].alarmRaised;
    }

    // reset alarm
    if ((digitalRead(PIN_BUTTON) == HIGH) && (alarmRaised)) {
      // muting alarm

      for (unsigned int iterator = 0; iterator < 5; iterator ++) {
          sensors[iterator].alarmRaised = false;
          sockets[iterator].alarmRaised = false;
      }

      
      //Serial.println(F("[button          ] Reset alarm."));
      
      digitalWrite(PIN_SPEAKER, LOW);
     
      return false;
    }

   
    
   if ((digitalRead(PIN_BUTTON) == HIGH) && (buttonPressedTimestamp == 0)) {
    
    buttonPressedTimestamp = millis() / 1000;
    
    //Serial.println(F("[button          ] Pressed"));
   }


   if ((digitalRead(PIN_BUTTON) == LOW) && (buttonPressedTimestamp > 0)) {


      unsigned int duration = (millis() / 1000) - buttonPressedTimestamp;

     // Serial.print(F("[button          ] Released after "));
     // Serial.print(duration);
     // Serial.println(F(" seconds."));

      buttonPressedTimestamp = 0;

      switch (duration) {

        // 1 - 4 seconds: feed pause
        case 1 ... 4 : {

        
          // serial debug information            
         // Serial.print(F("[button          ] Action: feed pause "));

          if (feedPause) {
         //   Serial.println(F("off"));
            feedPause = false;
           // iterate through all sensors
            for (int sensorIterator = 0; sensorIterator < 5; sensorIterator ++) {
              sensors[sensorIterator].stateChanged = SENSORSTATE_CHANGED;
            }
          } else {
           // Serial.println(F("on"));
            feedPause = true;
          }
     
          
        } break;
        
        // 5 - 14 seconds: Pond[CTRL] hard reset
        // please note that all settings, including calibration data, will be reset to parameters
        // set in settings.h
        case 5 ... 14: {
       
           // serial debug information
          // Serial.println(F("[button          ] Action: hard reset."));

           // reset all EEPROM data
           hardReset();

           // leave function with value true, indicating settings should be reloaded
           return true;

        } break;

        // 15 - 60 seconds: WiFi module hard reset
        case 15 ... 60: {

            /**
             * CODE SHOULD GO HERE
             */
          //   Serial.println(F("[button          ] Action: WiFi module hard reset"));

             // set special command to 112 (hard reset)
             specialCommandToWiFiModule = 112;

        } break;
      }
    
   }



   return false;

  
}
