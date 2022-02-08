#include "arduino.h"
#include "convert.h"


String byteArrayToString(byte byteArray[], char length) {
  String result = "";
  for (char iterator = 0; iterator < length; iterator ++) {
    result += (char) byteArray[iterator];
  }
  return result;
}

void unsignedIntToCharArray(unsigned int source, byte destination[2]) {
  destination[0] = source >> 8;
  destination[1] = source & 255;
}

void charArrayToUnsignedInt(byte source[2], unsigned int &destination) {
  destination = (source[0] << 8) + source[1];
}

/*
void floatToInt(float source, unsigned int &destination) {
  int beforeSeparator = source;
  source = ((source - beforeSeparator) * 1000);
  int afterSeparator = round(source / 10);
  destination = (beforeSeparator << 8) + afterSeparator;
}*/

void unsignedIntToFloat(unsigned int source, float &destination) {
  int wholepart = source >> 8;
  int decimalpart = source & 255;

  destination = (float) (((wholepart * 100) + decimalpart) / 100);
}



float mapf(float value, float istart, float istop, float ostart, float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
}
