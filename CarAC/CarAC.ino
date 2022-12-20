#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "ADC.h"
#include "LCD.h"
#include "keypad.h"
#include "pwm.h"
#include "uart.h"
#include "I2c.h"
#include "Rtc.h"
#include "Eeprom.h"

#define BV(bit) (1 << bit)
#define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))
#define loop_until_bit_is_set(sfr, bit) \
  do { \
  } while (bit_is_clear(sfr, bit))
#define loop_until_bit_is_clear(sfr, bit) \
  do { \
  } while (bit_is_set(sfr, bit))

#define setBit(sfr, bit) (_SFR_BYTE(sfr) |= (1 << bit))
#define clearBit(sfr, bit) (_SFR_BYTE(sfr) &= ~(1 << bit))
#define toggleBit(sfr, bit) (_SFR_BYTE(sfr) ^= (1 << bit))

#define Dio_SetPinDirection(sfr, bit, direction) ((direction == 1) ? (setBit(sfr, bit)) : (clearBit(sfr, bit)))
#define Dio_SetPinState(sfr, bit, state) ((state == 1) ? (setBit(sfr, bit)) : (clearBit(sfr, bit)))
#define Dio_GetPinState(sfr, bit) (bit_is_set(sfr, bit))

/*components:
1. keypad (pins already defined 2, 3, 11, 12, 13, A3)             
2. lcd (pins already defined 4, 5, 6, 7, 8, 9)
3. temperature sensor (A0)
4. servo (pins already defined 10)
5. buzzer (A1)
6. led (A2)
7. uart (pins already defined 0, 1)
8. rtc & EEPROM (pins already defined A4, A5)
*/

/*LCD Format:
HH:MM MM-DD-YY
[]C-[]-[]-[]  FL
*/

#define LM_DDR DDRC
#define LM_PORT PORTC
#define LM_CH 0

#define buzzerDDR DDRC
#define buzzerPORT PORTC
#define buzzer 1

#define ledDDR DDRC
#define ledPORT PORTC
#define led 2

#define LOWER_ADD 0
#define UPPER_ADD 1

//Global variables
int lower = 30;   //lower temperature threshold
int upper = 50;   //upper temperature threshold
int window = 30;  //current pwm of servo
int temp = 0;     //current temperature
int key = 0;      //pressed key

bool lock = false;
bool flasher = false;
bool buzz = false;

char s[4];  //string buffer

void init();

int main(void) {

  init();

  while (1) {
    //keypad checking
    key = get_key();
    if (key != 0) {

      if ((key == 1) and (!lock) and (window < 80)) { //window up button (increment by 10 unless locked) (max: 80)
        Uart_SendString("\nButton 1 pressed\n");
        window += 10;
        set_dutyCycle(window);
        itoa(window, s, 10);
        Uart_SendString("The user pressed the window up button. Current window state: ");
        Uart_SendString(s);
        Uart_SendString("\n");
      } else if ((key == 2) and ((lower + 1) < upper)) {  //lower temperature threshold up button (max value: higher temperature threshold - 1)
        Uart_SendString("\nButton 2 pressed\n");
        lower += 1;
        EEP_Write(LOWER_ADD, lower);
        itoa(lower, s, 10);
        LCD_String_xy(1, 4, s);
        Uart_SendString("The user pressed the low temperature threshold up button. Current lower temperature threshold: \n");
        Uart_SendString(s);
        Uart_SendString("\n");
      } else if (key == 3) {  //upper temperature threshold up button
        Uart_SendString("\nButton 3 pressed\n");
        upper += 1;
        EEP_Write(UPPER_ADD, upper);        
        itoa(upper, s, 10);
        LCD_String_xy(1, 7, s);
        Uart_SendString("The user pressed the high temperature threshold up button. Current upper temperature threshold: ");
        Uart_SendString(s);
        Uart_SendString("\n");
      } else if ((key == 4) and (!lock) and (window >= 40)) { //window down button (decrement by 10 unless locked) (min: 30)
        Uart_SendString("\nButton 4 pressed\n");
        window -= 10;
        set_dutyCycle(window);
        itoa(window, s, 10);
        Uart_SendString("The user pressed the window down button. Current window state: ");
        Uart_SendString(s);
        Uart_SendString("\n");
      } else if (key == 5) {  //lower temperature threshold down button
        Uart_SendString("\nButton 5 pressed\n");
        lower -= 1;
        EEP_Write(LOWER_ADD, lower);
        itoa(lower, s, 10);
        LCD_String_xy(1, 4, s);
        Uart_SendString("The user pressed the low temperature threshold down button. Current lower temperature threshold: ");
        Uart_SendString(s);
        Uart_SendString("\n");
      } else if ((key == 6) and (upper - 1 > lower)) {  //upper temperature threshold down button (min: lower temperature threshold + 1)
        Uart_SendString("\nButton 6 pressed\n");
        upper -= 1;
        EEP_Write(UPPER_ADD, upper); 
        itoa(upper, s, 10);
        LCD_String_xy(1, 7, s);
        Uart_SendString("The user pressed the high temperature threshold down button. Current upper temperature threshold: ");
        Uart_SendString(s);
        Uart_SendString("\n");
      } else if (key == 7) {  //Window lock button
        Uart_SendString("\nButton 7 pressed\n");
        lock = !lock;
        if (lock) {
          LCD_String_xy(1, 13, "L");
          Uart_SendString("The user pressed the lock button. Now Windows are locked\n");
        } else {
          LCD_String_xy(1, 13, " ");
          Uart_SendString("The user pressed the lock button. Now windows are unlocked\n");
        }
      } else if (key == 8) {  //Full Light button
        Uart_SendString("\nButton 8 pressed\n");
        flasher = !flasher;
        if (flasher) {
          Dio_SetPinState(ledPORT, led, 1);
          Uart_SendString("The user pressed the led button. Lights are now on.\n");
          LCD_String_xy(1, 15, "F");
        } else {
          Dio_SetPinState(ledPORT, led, 0);
          Uart_SendString("The user pressed the led button. Lights are now off.\n");
          LCD_String_xy(1, 15, " ");
        }
      } else if (key == 9) {  //Honk button
        Uart_SendString("\nButton 9 pressed\n");
        buzz = !buzz;
        if (buzz) {
          Dio_SetPinState(buzzerPORT, buzzer, 1);
          Uart_SendString("The user pressed the buzzer button. Buzzer on.\n");
        } else {
          Dio_SetPinState(buzzerPORT, buzzer, 0);
          Uart_SendString("The user pressed the buzzer button. Buzzer off.\n");
        }
      }
    }
    _delay_ms(100);

    //reading the ambient temperature    
    temp = ADC_Read(LM_CH);
    itoa(temp, s, 10);
    LCD_String_xy(1, 0, s);
    Uart_SendString("Value of temperature now is: ");
    Uart_SendString(s);
    Uart_SendString("\n");
    if (temp > upper) {
      window = 30;
      set_dutyCycle(window);
      LCD_String_xy(1, 10, "AC");
      Uart_SendString("Temperature is too high. Window closed and AC opened.");
    } else {
      LCD_String_xy(1, 10, "  ");
    }
    _delay_ms(20);
    if (temp < lower) {
      window = 80;
      set_dutyCycle(window);
      Uart_SendString("Temperature is too low. Window opened.");
    }
    //reading and printing the date and time to the uart and LCD
    read_clock();
  }
  return 0;
}

void init() {
  ADC_Init();
  LCD_Init();
  keypad_init();
  pwm_init();
  Uart_Init();
  I2C_Init();
  //Starting the clock counter offset
  RTC_Clock_Write(1, 40, 00, 1);
  RTC_Calendar_Write(2, 18, 30, 22);

  Dio_SetPinDirection(ledDDR, led, 1);
  Dio_SetPinDirection(buzzerDDR, buzzer, 1);
  Dio_SetPinDirection(LM_DDR, LM_CH, 0);

  Dio_SetPinState(ledPORT, led, 0);
  Dio_SetPinState(buzzerPORT, buzzer, 0);

  //Retrieving the previous values of temperature thresholds
  lower = EEP_Read(LOWER_ADD);
  upper = EEP_Read(UPPER_ADD);

  //read the date & log on LCD and uart
  LCD_Clear();
  _delay_ms(100);

  //getting the date and time for the first time
  read_clock();
  
  LCD_String_xy(1, 2, "C-");
  LCD_String_xy(1, 6, "-");
  LCD_String_xy(1, 9, "-");
  LCD_String_xy(1, 4, lower);
  LCD_String_xy(1, 7, upper);
  _delay_ms(200);
  Uart_SendString("System Started!\n");
}
