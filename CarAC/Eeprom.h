#ifndef __EEPROM__
#define __EEPROM__ 

/* function for clock */
void EEP_Write(char address, char value);

char EEP_Read(char address);

#endif /*#ifndef __EEPROM__*/