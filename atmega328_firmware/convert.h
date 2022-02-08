#ifndef convert_h
#define convert_h

String byteArrayToString(byte byteArray[], char length);
void unsignedIntToCharArray(unsigned int source, byte destination[2]);
void charArrayToUnsignedInt(byte source[2], unsigned int &destination);
void floatToInt(float source, unsigned int &destination);
float mapf(float value, float istart, float istop, float ostart, float ostop);
void unsignedIntToFloat(unsigned int source, float &destination);
#endif
