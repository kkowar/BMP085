/**
* BMP085 Library
*
*
* Manages communication with BMP085 barometric pressure & temperature sensor
*
*/

#ifndef BMP085_h
#define BMP085_h

#include "WProgram.h"

class BMP085Class
{
  public:
    float temp, press;
	
	void getCalData();
	void readSensor();
	
  private:
    long read_ut();
	long read_up();
	void write_register(byte r, byte v);
	unsigned int read_int_register(byte r);
};

extern BMP085Class BMP085;

#endif
