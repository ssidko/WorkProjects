
int I2C3_Initialize(void);
void I2C3_Write(char slave_addr, char *buf, int len);
void I2C3_Read(char slave_addr, char *buf, int len);

int I2C1_Initialize(void);
void I2C1_Write(char slave_addr, char *buf, int len);
void I2C1_Read(char slave_addr, char *buf, int len);
