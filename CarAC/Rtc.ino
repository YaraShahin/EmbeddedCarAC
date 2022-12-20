#include "I2c.h"
#include "Rtc.h"
#include "LCD.h"

int second,minute,hour,day,date,month,year;
/* function for clock */
void RTC_Clock_Write(char _hour, char _minute, char _second, char day_night_state)
{
	_hour |= day_night_state;
	I2C_Start(Device_Write_address);/* Start I2C communication with RTC */
	I2C_Write(0);			/* Write 0 address for second */
	I2C_Write(_second);		/* Write second on 00 location */
	I2C_Write(_minute);		/* Write minute on 01(auto increment) location */
	I2C_Write(_hour);		/* Write hour on 02 location */
	I2C_Stop();			/* Stop I2C communication */
}

/* function for calendar */
void RTC_Calendar_Write(char _day, char _date, char _month, char _year)
{
	I2C_Start(Device_Write_address);/* Start I2C communication with RTC */
	I2C_Write(3);			/* Write 3 address for day */
	I2C_Write(_day);		/* Write day on 03 location */
	I2C_Write(_date);		/* Write date on 04 location */
	I2C_Write(_month);		/* Write month on 05 location */
	I2C_Write(_year);		/* Write year on 06 location */
	I2C_Stop();			/* Stop I2C communication */
}

char IsItPM(char hour_)
{
	if(hour_ & (AM_PM))
	return 1;
	else
	return 0;
}

void RTC_Read_Clock(char read_clock_address)
{
	I2C_Start(Device_Write_address);/* Start I2C communication with RTC */
	I2C_Write(read_clock_address);	/* Write address to read */
	I2C_Repeated_Start(Device_Read_address);/* Repeated start with device read address */

	second = I2C_Read_Ack();	/* Read second */
	minute = I2C_Read_Ack();	/* Read minute */
	hour = I2C_Read_Nack();		/* Read hour with Nack */
	I2C_Stop();			/* Stop i2C communication */
}

void RTC_Read_Calendar(char read_calendar_address)
{
	I2C_Start(Device_Write_address);
	I2C_Write(read_calendar_address);
	I2C_Repeated_Start(Device_Read_address);

	day = I2C_Read_Ack();		/* Read day */ 
	date = I2C_Read_Ack();		/* Read date */
	month = I2C_Read_Ack();		/* Read month */
	year = I2C_Read_Nack();		/* Read the year with Nack */
	I2C_Stop();			/* Stop i2C communication */
}

void read_clock(){
  char buffer[20];
    char* days[7]= {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
      RTC_Read_Clock(0);  /* Read clock with second add. i.e location is 0 */
    if (hour & TimeFormat12)  
    {
      sprintf(buffer, "%02x:%02x:%02x  ", (hour & 0b00011111), minute, second);
      if(IsItPM(hour))
      strcat(buffer, "PM\n");
      else
      strcat(buffer, "AM\n");
      Uart_SendString(buffer);
    }
    
    else
    {
      sprintf(buffer, "%02x:%02x ", (hour & 0b00011111), minute);
      LCD_String_xy(0, 0, buffer);
      Uart_SendString(buffer);
    }
    RTC_Read_Calendar(3); /* Read calendar with day address i.e location is 3 */
    sprintf(buffer, "%02d/%02d/%02d", date, month, year);
    strcat(buffer, "\n");
    LCD_String_xy(0, 6, buffer);
    LCD_String_xy(0, 14,"  ");
    Uart_SendString(buffer);
}
