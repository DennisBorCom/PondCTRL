/**
 * 
 * 
 * Socket behaviour: sockets can be switched according to socket modes, specified in settings.h. Sockets can raise an alarm if
 * the duration of switching surpasses the specified max on time. after raising an alarm, the socket will automatically turn
 * off until the state changes.
 * 
 */


#include "socket.h"
#include "arduino.h"
#include "convert.h"


/**
 * 
 * 
 * 
 * TODO_ ALWAYS ON / OFF
 * 
 * 
 * 
 */
void setSocketSettings(Socket &socket, char pinNumber, char socketMode, bool socketAffectedByFeedPause, unsigned int socketMaxOnTime, byte name[16]) {
  socket.pinNumber = pinNumber;
  socket.mode = socketMode;
  socket.affectedByFeedPause = socketAffectedByFeedPause;
  socket.maxOnTime = socketMaxOnTime;
  
  if (socket.mode == SOCKET_MODE_ALWAYS_OFF) {
    
  //  setPinHighOrLow(socket, sensors[sensorIterator], SOCKET_MODE_PH1_LOW, SENSORSTATE_LOW);
    
    digitalWrite(socket.pinNumber, LOW);
  }

  if (socket.mode == SOCKET_MODE_ALWAYS_ON) {
    digitalWrite(socket.pinNumber, HIGH);
  }

  for (unsigned int copier = 0; copier < 16; copier ++) {
    socket.name[copier] = name[copier];
    
  }

  //Serial.print(F("["));
  //Serial.print(byteArrayToString(socket.name, 16));
  //Serial.print(F("] Changed settings to: "));
  //Serial.print((int) pinNumber);
  //Serial.print(F(" (hardware pin), "));
  //Serial.print((int) socketMode);
  //Serial.print(F(" (socket mode), "));
  //Serial.print(socketAffectedByFeedPause);
  //Serial.print(F(" (affected by feed pause), "));
  //Serial.print(socketMaxOnTime);
  //Serial.println(F(" (max on time)"));
  //*/
}

void detectSocketWarnings(Socket sockets[5]) {


/*
  
  // iterate through all sockets
  for (int socketIterator = 0; socketIterator < 5; socketIterator ++) {

     // only check sockets with maxOnTime parameter set (0 = disabled)
     if ((sockets[socketIterator].maxOnTime > 0)) {

        // if max time passed, socket is on and alarm not yet raised
        if ((sockets[socketIterator].maxOnTime < ((millis() / 1000) - sockets[socketIterator].lastSwitchTimestamp)) && (digitalRead(sockets[socketIterator].pinNumber) == HIGH) && (!sockets[socketIterator].alarmRaised)) {

          // write debug information to serial
         // Serial.print(F("["));
        //  Serial.print(byteArrayToString(sockets[socketIterator].name, 16));
        //  Serial.println(F("] Alarm, turning off."));

          // turn off socket
          digitalWrite(sockets[socketIterator].pinNumber, LOW);

          // set alarm to true;
          sockets[socketIterator].alarmRaised = true;


        
       
        } 
     }   
  } 

  */
}

void switchSockets(Sensor sensors[5], Socket sockets[5], bool feedPause) {

/*


  // always on/off
  for (unsigned int socketIterator = 0; socketIterator < 5; socketIterator ++) {

      if (!sockets[socketIterator].override) {

        if (sockets[socketIterator].mode == SOCKET_MODE_ALWAYS_OFF) {
           
            digitalWrite(sockets[socketIterator].pinNumber, LOW);
        }
  
        if (sockets[socketIterator].mode == SOCKET_MODE_ALWAYS_ON) {
          digitalWrite(sockets[socketIterator].pinNumber, HIGH);
        }

      }
    
  }


  // iterate through all sensors
  for (unsigned int sensorIterator = 0; sensorIterator < 5; sensorIterator ++) {

    // only take action if the sensor state is changed
    if (sensors[sensorIterator].stateChanged == SENSORSTATE_CHANGED) {
      
        

        // switch statement for the sensor iterator, corresponding 
        // to the sensor numbers
        switch (sensorIterator) {

          // pH sensor # 1
          case SENSOR_PH1: {

            // iterate through all sockets
            for (int socketIterator = 0; socketIterator < 5; socketIterator ++) {

              // handle switching for ph1 low and high
              setPinHighOrLow(sockets[socketIterator], sensors[sensorIterator], SOCKET_MODE_PH1_LOW, SENSORSTATE_LOW, feedPause);
              setPinHighOrLow(sockets[socketIterator], sensors[sensorIterator], SOCKET_MODE_PH1_HIGH, SENSORSTATE_HIGH, feedPause);  
              
            }
  
          } break;

          // pH sensor #2
          case SENSOR_PH2: {

            // iterate through all sockets
            for (int socketIterator = 0; socketIterator < 5; socketIterator ++) {

              // handle switching for ph2 low and high
              setPinHighOrLow(sockets[socketIterator], sensors[sensorIterator], SOCKET_MODE_PH2_LOW, SENSORSTATE_LOW, feedPause);
              setPinHighOrLow(sockets[socketIterator], sensors[sensorIterator], SOCKET_MODE_PH2_HIGH, SENSORSTATE_HIGH, feedPause);

            }
    
          } break;

          // temp sensor #1
          case SENSOR_TEMP1: {

            // iterate through all sockets
            for (int socketIterator = 0; socketIterator < 5; socketIterator ++) {

              // handle switching for temp1 low and high
              setPinHighOrLow(sockets[socketIterator], sensors[sensorIterator], SOCKET_MODE_TEMP1_LOW, SENSORSTATE_LOW, feedPause);
              setPinHighOrLow(sockets[socketIterator], sensors[sensorIterator], SOCKET_MODE_TEMP1_HIGH, SENSORSTATE_HIGH, feedPause);
              
            }
            
          } break;

          // temp sensor #2
          case SENSOR_TEMP2: {

            // iterate through all sockets
            for (int socketIterator = 0; socketIterator < 5; socketIterator ++) {

              // handle switching for temp2 low and high
              setPinHighOrLow(sockets[socketIterator], sensors[sensorIterator], SOCKET_MODE_TEMP2_LOW, SENSORSTATE_LOW, feedPause);
              setPinHighOrLow(sockets[socketIterator], sensors[sensorIterator], SOCKET_MODE_TEMP2_HIGH, SENSORSTATE_HIGH, feedPause);

        
            } 
            
          } break;

          // level switch
          case SENSOR_LEVEL: {

            // iterate through all sockets
            for (int socketIterator = 0; socketIterator < 5; socketIterator ++) {

              // handle switching for level low and high
              setPinHighOrLow(sockets[socketIterator], sensors[sensorIterator], SOCKET_MODE_LEVEL_LOW, SENSORSTATE_LOW, feedPause);
              setPinHighOrLow(sockets[socketIterator], sensors[sensorIterator], SOCKET_MODE_LEVEL_HIGH, SENSORSTATE_HIGH, feedPause);

            }
            
          } break;
          
        }

       

        
    } else if (feedPause) {

      // iterate through all sockets
      for (int socketIterator = 0; socketIterator < 5; socketIterator ++) {

        if (sockets[socketIterator].affectedByFeedPause && !sockets[socketIterator].override) {
          
            digitalWrite(sockets[socketIterator].pinNumber, LOW);
        }

      }
      
    }
  }

  // detect warnings (sockets which are switched on longer than the maximum time specified)
  detectSocketWarnings(sockets);

  */
}

void setPinHighOrLow(Socket &socket, Sensor &sensor, int modeSet, int requiredState, bool feedPause) {

  /*
 

  if (feedPause && socket.affectedByFeedPause && socket.mode == modeSet) {

     // write debug information to serial
  //  Serial.print(F("["));
  //  Serial.print(byteArrayToString(socket.name, 16));
  //  Serial.println(F("] Feedpause active - Socket switching aborted"));
 
    return;
  }


  // current mode equals the mode set and the sensor state equals the required state
  // socket not overridden
  if ((socket.mode == modeSet) && (sensor.state == requiredState) && (!socket.override)) {

    // write debug information to serial
   // Serial.print(F("["));
   //Serial.print(byteArrayToString(socket.name, 16));
  // Serial.println(F("] On"));

    // turn the socket on by activating the relay
    digitalWrite(socket.pinNumber, HIGH);

    // set timestamp in seconds
    socket.lastSwitchTimestamp = millis() / 1000;

    // reset to unchanged, preventing unnecessary switching
    // sensor switching only takes place if the stateChanged parameter is set to CHANGED
    sensor.stateChanged = SENSORSTATE_UNCHANGED;  
        
   
  // curent mode equals the mode set but the sensor state is not right 
  // socket not overridden
  } else if ((socket.mode == modeSet) && (!socket.override)) {

    // write debug information to serial
  //  Serial.print(F("["));
  //  Serial.print(byteArrayToString(socket.name, 16));
  //  Serial.println(F("] Off"));

    // turn the socket off by deactivating the relay
    digitalWrite(socket.pinNumber, LOW);

    // set timestamp in seconds
    socket.lastSwitchTimestamp =  millis() / 1000;

     // reset to unchanged, preventing unnecessary switching
     // sensor switching only takes place if the stateChanged parameter is set to CHANGED
     sensor.stateChanged = SENSORSTATE_UNCHANGED;  
    
  }*/
 
        
}
