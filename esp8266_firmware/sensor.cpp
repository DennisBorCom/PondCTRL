/**
 * Pond[CTRL]™ WiFi module firmware, © Copyright Dennis Bor, all rights reserved. Pond[CTRL]™ is a trademark of Dennis Bor. 
 */

 #include "arduino.h"
 #include "sensor.h"

/**
 * Sets the current sensor value. values are send to the iot api by taking averages
 * from the last ten values.
 * 
 * @param           value              value to set
 * 
 * @return          this function does not produce a return value
 */
void Sensor::setValue(unsigned int value) {

  // sensor is a switch - no value averaging
  if (isSwitch) {

    // store value and set the numbers of values stored to 1
    values[0] = value;
    valuesStored = 1;
  }

/*
  if (valuesStored == 0) {
      values[0] = value;
      valuesStored = 1;
      return;
  }
*/
  

  if (valuesStored > 0) {

        // given value is more than 10% different to average value?
        if ((value < values[(valuesStored - 1)] * 0.90) || (value > values[(valuesStored - 1)] * 1.10)) {
          
          // reject value (do nothing)
          // relay switching can cause interference in sensor readings. this is why 
          // sensor readings with big differences are filtered (not saved)
          return;

      }    
  }
  
  // increment through the value array
  for (unsigned int iterator = 1; iterator < NUMBEROFVALUES; iterator ++) {
      
     // shift values in the array one position left
     // deleting the oldest value
     values[(iterator - 1)] = values[iterator];
  }

  // less than max values stored?
  if (valuesStored < NUMBEROFVALUES) {

    // increment the counter
    // this counter is required to be able to calculate the proper
    // average value, before ten values are registered
    valuesStored ++;
  }

  // register the new value at the last position
  values[(valuesStored - 1)] = value;
}


/**
 * Returns the average sensor value calculated from last ten readings
 * 
 * @return          average sensor value from last ten readings
 */
 unsigned int Sensor::getValue() {

  // unsigned long to store total calculated value
  unsigned long totalValue = 0;

  // iterate through the number of values stored
  for (unsigned int iterator = 0; iterator < valuesStored; iterator ++) {

       
      // add the current value to the total value
      totalValue += values[iterator];
    
  }

  // return the calculated value (total value divided by the number of
  // values stored)
  return totalValue / valuesStored;
}
