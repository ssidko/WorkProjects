#include <Wire.h>

#define LED_PIN                 13 
#define TRANSFER_TIMEOUT        1000

char is_transfer_begin =  false;
unsigned long current_time = 0;
unsigned long last_data_in = 0;

void setup ()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin();  
  Serial.println("---==== Board started ====---");
}

void loop ()
{
  if (is_transfer_begin == true) {
    current_time = millis();
    if ((current_time - last_data_in) > TRANSFER_TIMEOUT) {
      Wire.endTransmission();
      is_transfer_begin = false;
      Serial.println("--- End transfer ---");
    }   
  }
}

void I2C_Write(byte val)
{
//  Wire.beginTransmission(4);
//  Wire.write(val);
//  Wire.endTransmission();
  if (!is_transfer_begin) {
    Wire.beginTransmission(4);
    is_transfer_begin = true;
    Serial.println("--- Begin transfer ---");
  }

  Serial.print("0x");
  Serial.println(val, HEX);
  
}

void serialEvent ()
{
  byte in_byte;
  int recived_bytes = Serial.available();
  last_data_in = millis();
  while(recived_bytes > 0) {
    in_byte = (char)Serial.read();
    I2C_Write(in_byte);
    --recived_bytes;
  }
}
