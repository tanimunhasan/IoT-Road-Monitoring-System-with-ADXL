#include <Wire.h> 
#define DEVICE (0x53) // Device address as specified in data sheet 
byte data[6]; 
char POWER_CTL = 0x2D; //Power Control Register 
char DATA_FORMAT = 0x31; 
char DATAX0 = 0x32; //X-Axis Data 0 
void setup()
{ 
Wire.begin(); // join i2c bus (address optional for master) 
Serial.begin(9600);// start serial for output. Make sure you set your Serial Monitor to the same! 
//Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register. 
writeTo(DATA_FORMAT, 0x01); 
//Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register. 
writeTo(POWER_CTL, 0x08); 
}
void loop() 
{ 
uint8_t howManyBytesToRead = 6; 
readFrom(DATAX0, howManyBytesToRead, data); 
int x = (((int)data[1]) << 8) | data[0]; 
int y = (((int)data[3]) << 8) | data[2]; 
int z = (((int)data[5]) << 8) | data[4];
Serial.print( x ); 
Serial.print(","); 
Serial.print( y ); 
Serial.print(","); 
Serial.print( z ); 
Serial.print(","); 
Serial.print("\n"); 
delay(100); 
} 
void writeTo(byte address, byte val) 
{
Wire.beginTransmission(DEVICE); // start transmission to device 
Wire.write(address); // send register address 
Wire.write(val); // send value to write 
Wire.endTransmission(); // end transmission 
}
// Reads num bytes starting from address register on device in to _buff array 
void readFrom(byte address, int num, byte data[]) 
{ 
  Wire.beginTransmission(DEVICE); // start transmission to device 
  Wire.write(address); // sends address to read from 
  Wire.endTransmission(); // end transmission 
  Wire.beginTransmission(DEVICE); // start transmission to device 
  Wire.requestFrom(DEVICE, num); // request 6 bytes from device 
  int i = 0; 
  while(Wire.available()) // device may send less than requested (abnormal) 
  { data[i] = Wire.read(); // receive a byte 
  i++; } 
  Wire.endTransmission(); // end transmission }
}
