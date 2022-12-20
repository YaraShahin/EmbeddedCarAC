#ifndef __RTC__
#define __RTC__ 

#define Device_Write_address	0xD0	/* Define RTC DS1307 slave write address */
#define Device_Read_address	    0xD1	/* Make LSB bit high of slave address for read */
#define TimeFormat12		    0x40	/* Define 12 hour format */
#define AM_PM			        0x20

/* function for clock */
void RTC_Clock_Write(char _hour, char _minute, char _second, char day_night_state);

void RTC_Calendar_Write(char _day, char _date, char _month, char _year);

char IsItPM(char hour_);

void RTC_Read_Clock(char read_clock_address);

void RTC_Read_Calendar(char read_calendar_address);

#endif /*#ifndef __RTC__*/