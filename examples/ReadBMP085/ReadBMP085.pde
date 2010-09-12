/*
*  Read the Barometric Pressure and Temperature values from BMP085 sensor
*
*/

#include <Wire.h>
#include <BMP085.h>

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  BMP085.getCalData();
}

void loop()
{
  BMP085.readSensor();
  
  Serial.print("Temperature(C): ");
  Serial.print(BMP085.temp, 1);
  Serial.print("     Pressure(kPa): ");
  Serial.println(BMP085.press);

  delay(60000);
}

