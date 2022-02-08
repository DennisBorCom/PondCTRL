#include "sensor.h"

/**
 * 
 * Note: converted values (unsigned ints) are required, because typecasting seems to interfere with i2c communication
 */

 
void setSensorValues(Sensor &sensor, unsigned int lowValue, unsigned int highValue, unsigned int threshold, byte name[16], unsigned int alarmLow, unsigned int alarmHigh, int correctionValue, unsigned int adcPH4, unsigned int adcPH7) {
  
  sensor.lowValue = lowValue;
  //floatToInt(lowValue, sensor.lowValueUnsignedInt);
  sensor.highValue = highValue;
  //floatToInt(highValue, sensor.highValueUnsignedInt);
  sensor.threshold = threshold;
  //floatToInt(threshold, sensor.thresholdUnsignedInt);
  sensor.alarmLow = alarmLow;
  //floatToInt(alarmLow, sensor.alarmLowUnsignedInt);
  sensor.alarmHigh = alarmHigh;
  //floatToInt(alarmHigh, sensor.alarmHighUnsignedInt);
  for (unsigned int copier = 0; copier < 16; copier ++) {
    sensor.name[copier] = name[copier];
    
  }

  sensor.correctionValue = correctionValue;
  sensor.adcPH4 = adcPH4;
  sensor.adcPH7 = adcPH7;

 /* Serial.print(F("["));
  Serial.print(byteArrayToString(sensor.name, 16));
  Serial.print(F("] Changed settings to: "));
  Serial.print(lowValue);
  Serial.print(F(" (low), "));
  Serial.print(highValue);
  Serial.print(F(" (high), "));
  Serial.print(threshold);
  Serial.print(F(" (threshold), "));
  Serial.print(alarmLow);
  Serial.print(F(" (alarm low), "));
  Serial.print(alarmHigh);
  Serial.println(F(" (alarm high)"));
  */
  //sensor.name = name;
}


/**
 * Returns the average sensor value calculated from last ten readings
 * 
 * @return          average sensor value from last ten readings
 *//*
 unsigned int getValue(Sensor sensor) {

  // unsigned long to store total calculated value
  unsigned long totalValue = 0;

  // iterate through the number of values stored
  for (unsigned int iterator = 0; iterator < sensor.valuesStored; iterator ++) {

    // add the current value to the total value
    totalValue += sensor.values[iterator];
  }

  // return the calculated value (total value divided by the number of
  // values stored)
  return totalValue / sensor.valuesStored;
}*/

/*void setValue(Sensor &sensor, unsigned int value) {

  if (sensor.onOffOnly) {
    sensor.valuesStored = 1;
    sensor.values[0] = value;
    return;
  }

  
  // currently at least one value is stored?
  if (sensor.valuesStored > 0) {

    // given value is more than 5% different to last value?
    if ((value < (sensor.values[(sensor.valuesStored - 1)] * 0.95)) || (value > sensor.values[(sensor.valuesStored - 1)] * 1.05)) {
      
      // reject value (do nothing)
      // relay switching can cause interference in sensor readings. this is why 
      // sensor readings with big differences are filtered (not saved)
      return;
    }
  }
  


  // increment through the value array
  for (unsigned int iterator = 1; iterator < 120; iterator ++) {
      
     // shift values in the array one position left
     // deleting the oldest value
     sensor.values[(iterator - 1)] = sensor.values[iterator];
  }

  // less than 10 values stored?
  if (sensor.valuesStored < 120) {

    // increment the counter
    // this counter is required to be able to calculate the proper
    // average value, before ten values are registered
    sensor.valuesStored ++;
  }

  // register the new value at the last position
  sensor.values[(sensor.valuesStored - 1)] = value;
}*/

void setState(Sensor &sensor, char state) {


    /**
   if (sensor.state != state) {

     // Serial.print(F("["));
      //Serial.print(byteArrayToString(sensor.name, 16));
     // Serial.println(F("] State changed"));

      sensor.stateChanged = SENSORSTATE_CHANGED;
      sensor.state = state;
  }*/

}

void calculateSensorState(Sensor &sensor) {



/*
      bool alarmRaised = false;
      
      if (!sensor.onOffOnly) { // **********************


   
      if ((sensor.value <= sensor.alarmLow) || (sensor.value >= sensor.alarmHigh)) {

          
           
                // set alarm to true;
                alarmRaised = true;
                
    
                
      
         
     
         }


      } else {

             
        
              if ((sensor.alarmLow == 100) && (sensor.value == 0)) {
                alarmRaised = true;
              }

          
              
              if ((sensor.alarmHigh == 100) && (sensor.value == 100)) {
                alarmRaised = true;
              }
        
        
      }

      if (alarmRaised) {

              sensor.alarmRaised = true;
        
        // write debug information to serial
            //    Serial.print(F("["));
           //     Serial.print(byteArrayToString(sensor.name, 16));
           //     Serial.println(F("] Alarm"));
      } else {

            sensor.alarmRaised = false;
        
      }
      
      


        if (sensor.state == SENSORSTATE_LOW) {
         
          if ((sensor.value >= (sensor.lowValue + sensor.threshold)) && !sensor.onOffOnly) {

              setState(sensor, SENSORSTATE_OK);
              return;
          }
            
        } else if (sensor.state == SENSORSTATE_HIGH) {

            if ((sensor.value <= (sensor.highValue - sensor.threshold)) && !sensor.onOffOnly) {

              setState(sensor, SENSORSTATE_OK);
              return;
            } 
         
        } else if (sensor.state == SENSORSTATE_OK) {

          if (sensor.value <= sensor.lowValue) {

                  setState(sensor, SENSORSTATE_LOW);
                return;
              }

              if (sensor.value >= sensor.highValue) {

                  setState(sensor, SENSORSTATE_HIGH);
                return;
              }
              
        } else {

          // intializer (unknown sensor states -> SENSORSTATE_OK);
          //setState(sensor, SENSORSTATE_OK);
        }

        if (sensor.onOffOnly) {

           if ((sensor.value == 0) && (sensor.state == SENSORSTATE_HIGH)) {
              setState(sensor, SENSORSTATE_LOW);
           }

           if ((sensor.value == 100) && (sensor.state == SENSORSTATE_LOW)) {
              setState(sensor, SENSORSTATE_HIGH);
           }
              
        }


       
  */
}
