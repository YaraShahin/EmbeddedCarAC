#include <avr/io.h>
#include <util/delay.h>
#include "pwm.h"

//works on pin 10 (PB2) -> OC1B
void pwm_init() {

  //configure pin as output
  Dio_SetPinDirection(DDRB, 2, 1);

  //Configure timer mode as Fast PWM (Non Inverted)
  setBit(TCCR1A, WGM00);
  setBit(TCCR1A, WGM01);
  clearBit(TCCR1B, WGM02);  

  //configure the timer as non-inverted
  setBit(TCCR1A, COM1B1);
  clearBit(TCCR1A, COM1B0);

  //Configure Prescaler 256
  setBit(TCCR1B, CS02);
  clearBit(TCCR1B, CS01);
  clearBit(TCCR1B, CS00);
}

//Configure duty cycle
void set_dutyCycle(int duty) {
  OCR1B = duty;
}
