#include <avr/io.h>
#include <util/delay.h>	  /* Include Delay header file */
#include "ADC.h"

void ADC_Init() {
  ADCSRA = ADCSRA | (1 << ADEN);
  ADCSRA = ADCSRA | (1 << ADPS0);
  ADCSRA = ADCSRA | (1 << ADPS1);
  ADCSRA = ADCSRA | (1 << ADPS2);

  ADMUX = ADMUX | (1 << REFS0);
  ADMUX = ADMUX & ~(1 << REFS1);
}

int ADC_Read(int channel){ 
  //resetting the adc before each reading to prevent interference of two channels at the MUX
  ADC_Init();
  
  ADMUX = ADMUX & ~(1 << MUX0);
  ADMUX = ADMUX & ~(1 << MUX1);
  ADMUX = ADMUX & ~(1 << MUX2);
  ADMUX = ADMUX & ~(1 << MUX3);
  ADMUX = ADMUX | channel;

  ADCSRA = ADCSRA | (1 << ADSC);

  while(ADCSRA & (1 << ADSC));

  return ADC;
}