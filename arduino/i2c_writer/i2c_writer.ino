#include <Wire.h>

#define LED_PIN                 13
#define CTP_RESET               6       
#define TRANSFER_TIMEOUT        1000
#define I2C_CTPM_ADDRESS        (byte)0x38

char is_transfer_begin =  false;
unsigned long current_time = 0;
unsigned long last_data_in = 0;

void ctp_reset(void)
{
  digitalWrite(CTP_RESET, LOW);
  delay(100);
  digitalWrite(CTP_RESET, HIGH);
  delay(100);
}

byte i2c_read_interface(byte addr, byte *buf, byte len)
{
  Wire.requestFrom(addr, len);
  byte i = 0;
  unsigned long last_rcv = millis();
  while (((i + 1) < len) && (millis() - last_rcv) < 500) {
    if (Wire.available()) {
      buf[i] = Wire.read();
      ++i;
      last_rcv = millis();
    }
  }
  if ((i + 1) != len) {
    Serial.println("Error read i2c interface");
    return false;
  } else {
    return true;  
  }
}

byte i2c_write_interface(byte addr, byte *buf, byte len)
{
  byte ret;
  Wire.beginTransmission(addr);
  ret = Wire.write(buf, len);
  if (ret != len) return false;
  Wire.endTransmission();
  return true;
}

byte fts_register_write(byte reg_name, byte bt_value)
{
  byte write_cmd[2] = {0};
  write_cmd[0] = reg_name;
  write_cmd[1] = bt_value;
  if ( i2c_write_interface(I2C_CTPM_ADDRESS, write_cmd, 2)) {
    return true;
  } else {
      Serial.println("Error write i2c interface");    
  }  
}

byte cmd_write(byte cmd, byte para1, byte para2, byte para3, byte num)
{
  byte write_cmd[4] = {0};
  write_cmd[0] = cmd;
  write_cmd[1] = para1;
  write_cmd[2] = para2;
  write_cmd[3] = para3;
  return i2c_write_interface(I2C_CTPM_ADDRESS, write_cmd, num);
}

byte byte_read(byte *buf, byte len)
{
  return i2c_read_interface(I2C_CTPM_ADDRESS, buf, len);
}

byte PrepareForUpgrade(void)
{
  byte i = 0;
  byte ret = 0;
  byte reg_val[2] = {0};

  /*********Step 1:Reset  CTPM *****/
  /*write 0xaa to register 0xfc*/
  delay(100);
  fts_register_write(0xfc,0xaa);
  delay(50);
  /*write 0x55 to register 0xfc*/
  fts_register_write(0xfc,0x55);
  Serial.println("Step 1: Reset CTPM test.");
  delay(30); 

  /*********Step 2:Enter upgrade mode *****/
  byte auc_i2c_write_buf[10] = {0};
  auc_i2c_write_buf[0] = 0x55;
  auc_i2c_write_buf[1] = 0xaa;
  i = 0;
  do {
    i++;
    ret = i2c_write_interface(I2C_CTPM_ADDRESS, auc_i2c_write_buf, 2);
    Serial.println("Step 2: Enter update mode.");
    delay(5);
  } while((false == ret) && i<5);
  
  /*********Step 3:check READ-ID***********************/
  /*send the opration head*/
  i = 0;
  do{
    if(i > 3)
    {
      cmd_write(0x07,0x00,0x00,0x00,1);
      return false;
    }
    /*read out the CTPM ID*/
    Serial.println("Step 3:check READ-ID");
    cmd_write(0x90,0x00,0x00,0x00,4);
    byte_read(reg_val,2);
    i++;
    delay(5);
    Serial.print("Step 3: CTPM ID,ID1 = 0x"); Serial.print(reg_val[0], HEX);
    Serial.print(", ID2 = 0x"); Serial.println(reg_val[1], HEX);    
  }while(reg_val[1] != 0x03);//while(reg_val[0] != 0x79 || reg_val[1] != 0x03);
  
}

void ctpm_read_id(void)
{
  byte reg = 0x01;
  i2c_write_interface(I2C_CTPM_ADDRESS, &reg, 1);
  byte out[10] = {0}; 
  i2c_read_interface(I2C_CTPM_ADDRESS, out, 1);
  Serial.print("GUEST_ID = 0x"); Serial.println(out[0], HEX);
}

void setup ()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(CTP_RESET, OUTPUT);
  Wire.begin();  

  ctp_reset();
  digitalWrite(LED_PIN, HIGH);  
  
  Serial.println("---==== Board started ====---");
  
  ctpm_read_id();
  //PrepareForUpgrade();
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
    Wire.beginTransmission(I2C_CTPM_ADDRESS);
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
