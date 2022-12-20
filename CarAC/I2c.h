#ifndef __I2C__
#define __I2C__ 

void I2C_Init();

uint8_t I2C_Start(char write_address);

uint8_t I2C_Repeated_Start(char read_address);

uint8_t I2C_Write(char data);

char I2C_Read_Ack();

char I2C_Read_Nack();

void I2C_Stop();

#endif /*#ifndef __I2C__*/