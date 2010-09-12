/**
* BMP085 Library
*
*
* Manages communication with BMP085 barometric pressure & temperature sensor
*
*/

#include "WProgram.h"

#include "Wire.h"
#include "wiring.h"
#include "BMP085.h"

const unsigned char oversampling_setting = 3; //oversamplig for measurement
const unsigned char pressure_waittime[4] = { 5, 8, 14, 26 };

//just taken from the BMP085 datasheet
int ac1;
int ac2;
int ac3;
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1;
int b2;
int mb;
int mc;
int md;



/* ================ Public methods ================ */
 
 void BMP085Class::getCalData()
{
  ac1 = read_int_register(0xAA);
  ac2 = read_int_register(0xAC);
  ac3 = read_int_register(0xAE);
  ac4 = read_int_register(0xB0);
  ac5 = read_int_register(0xB2);
  ac6 = read_int_register(0xB4);
   b1 = read_int_register(0xB6);
   b2 = read_int_register(0xB8);
   mb = read_int_register(0xBA);
   mc = read_int_register(0xBC);
   md = read_int_register(0xBE);
}
 
/*
 * Fetch values from the sensor
 */
void BMP085Class::readSensor()
{
  long ut = read_ut();
  long up = read_up();
  long x1, x2, x3, b3, b5, b6, p;
  unsigned long b4, b7;

  //calculate the temperature
  x1 = ((long)ut - ac6) * ac5 >> 15;
  x2 = ((long) mc << 11) / (x1 + md);
  b5 = x1 + x2;
  temp = ((b5 + 8) >> 4) / 10.0;

  //calculate the pressure
  b6 = b5 - 4000;
  x1 = (b2 * (b6 * b6 >> 12)) >> 11;
  x2 = ac2 * b6 >> 11;
  x3 = x1 + x2;

  if (oversampling_setting == 3) b3 = ((int32_t) ac1 * 4 + x3 + 2) << 1;
  if (oversampling_setting == 2) b3 = ((int32_t) ac1 * 4 + x3 + 2);
  if (oversampling_setting == 1) b3 = ((int32_t) ac1 * 4 + x3 + 2) >> 1;
  if (oversampling_setting == 0) b3 = ((int32_t) ac1 * 4 + x3 + 2) >> 2;

  x1 = ac3 * b6 >> 13;
  x2 = (b1 * (b6 * b6 >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (ac4 * (uint32_t) (x3 + 32768)) >> 15;
  b7 = ((uint32_t) up - b3) * (50000 >> oversampling_setting);
  p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4) * 2;

  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  press = (p + ((x1 + x2 + 3791) >> 4)) / 1000.0;
}



/* ================ Private methods ================ */

/*
* Reads the current temperature value
*/
long BMP085Class::read_ut()
{
  write_register(0xf4,0x2e);
  delay(5); //longer than 4.5 ms
  return read_int_register(0xf6);
}
 
/*
* Reads current pressure value
*/
long BMP085Class::read_up()
{
  write_register(0xf4,0x34+(oversampling_setting<<6));
  delay(pressure_waittime[oversampling_setting]);

  unsigned char msb, lsb, xlsb;
  
  Wire.beginTransmission(0X77);
  Wire.send(0xf6); // register to read
  Wire.endTransmission();

  Wire.requestFrom(0X77, 3); // read a byte
  while(!Wire.available())
  {
    // waiting
  }
  msb = Wire.receive();
  
  while(!Wire.available())
  {
    // waiting
  }
  lsb |= Wire.receive();
  
  while(!Wire.available())
  {
    // waiting
  }
  xlsb |= Wire.receive();

  return (((long)msb<<16) | ((long)lsb<<8) | ((long)xlsb)) >>(8-oversampling_setting);
}

/*
 * Write single byte to a device register
*/
void BMP085Class::write_register(byte r, byte v)
{
  Wire.beginTransmission(0X77);
  Wire.send(r);
  Wire.send(v);
  Wire.endTransmission();
}

/*
 * Read a register from the device
 */
unsigned int BMP085Class::read_int_register(byte r)
{
  byte msb, lsb;

  Wire.beginTransmission(0X77);
  Wire.send(r); // register to read
  Wire.endTransmission();

  Wire.requestFrom(0X77, 2); // read a byte
  while(!Wire.available())
  {
    // waiting
  }
  msb = Wire.receive();

  while(!Wire.available())
  {
    // waiting
  }
  lsb = Wire.receive();
  
  return (((int)msb<<8) | ((int)lsb));
}

BMP085Class BMP085;
