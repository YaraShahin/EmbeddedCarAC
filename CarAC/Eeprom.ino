#include "I2c.h"
#include "Eeprom.h"
#include<avr/eeprom.h>

/* function for clock */
void EEP_Write(char address, char value)
{
	eeprom_write_byte ((uint8_t*) address, value); 
}

char EEP_Read(char address)
{
	char byteRead;
	byteRead = eeprom_read_byte((uint8_t*)address); // read the byte in location 23
	return byteRead;
}
