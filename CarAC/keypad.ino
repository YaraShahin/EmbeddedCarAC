#include <avr/io.h>
#include <util/delay.h>
#include "keypad.h"

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

//row pins: (D, 2), (D, 3), (B, 3)
//col pins: (B, 4), (B, 5), (C, 3)
#define r1DDR DDRD
#define r1PORT PORTD
#define r1PIN PIND
#define r1 2

#define r2DDR DDRD
#define r2PORT PORTD
#define r2PIN PIND
#define r2 3

#define r3DDR DDRB
#define r3PORT PORTB
#define r3PIN PINB
#define r3 3

#define c1DDR DDRB
#define c1PORT PORTB
#define c1PIN PINB
#define c1 4

#define c2DDR DDRB
#define c2PORT PORTB
#define c2PIN PINB
#define c2 5

#define c3DDR DDRC
#define c3PORT PORTC
#define c3PIN PINC
#define c3 3

#define rows 3
#define cols 3

const int keys[rows][cols] = {
  { 1, 2, 3 },
  { 4, 5, 6 },
  { 7, 8, 9 },
};

void keypad_init() {
  Dio_SetPinDirection(r1DDR, r1, 0);
  Dio_SetPinDirection(r2DDR, r2, 0);
  Dio_SetPinDirection(r3DDR, r3, 0);

  Dio_SetPinState(r1PORT, r1, 1);
  Dio_SetPinState(r2PORT, r2, 1);
  Dio_SetPinState(r3PORT, r3, 1);

  Dio_SetPinDirection(c1DDR, c1, 1);
  Dio_SetPinDirection(c2DDR, c2, 1);
  Dio_SetPinDirection(c3DDR, c3, 1);
}

int get_key() {
  int k = 0;

  //col 1
  Dio_SetPinState(c1PORT, c1, 0);
  _delay_ms(10);
  if (!Dio_GetPinState(r1PIN, r1)) {
    while (!Dio_GetPinState(r1PIN, r1))
      ;
    k = keys[0][0];
  }
  if (!Dio_GetPinState(r2PIN, r2)) {
    while (!Dio_GetPinState(r2PIN, r2))
      ;
    k = keys[1][0];
  }
  if (!Dio_GetPinState(r3PIN, r3)) {
    while (!Dio_GetPinState(r3PIN, r3))
      ;
    k = keys[2][0];
  }
  Dio_SetPinState(c1PORT, c1, 1);

  //col 2
  Dio_SetPinState(c2PORT, c2, 0);
  _delay_ms(10);
  if (!Dio_GetPinState(r1PIN, r1)) {
    while (!Dio_GetPinState(r1PIN, r1))
      ;
    k = keys[0][1];
  }
  if (!Dio_GetPinState(r2PIN, r2)) {
    while (!Dio_GetPinState(r2PIN, r2))
      ;
    k = keys[1][1];
  }
  if (!Dio_GetPinState(r3PIN, r3)) {
    while (!Dio_GetPinState(r3PIN, r3))
      ;
    k = keys[2][1];
  }
  Dio_SetPinState(c2PORT, c2, 1);

  //col 3
  Dio_SetPinState(c3PORT, c3, 0);
  _delay_ms(10);
  if (!Dio_GetPinState(r1PIN, r1)) {
    while (!Dio_GetPinState(r1PIN, r1))
      ;
    k = keys[0][2];
  }
  if (!Dio_GetPinState(r2PIN, r2)) {
    while (!Dio_GetPinState(r2PIN, r2))
      ;
    k = keys[1][2];
  }
  if (!Dio_GetPinState(r3PIN, r3)) {
    while (!Dio_GetPinState(r3PIN, r3))
      ;
    k = keys[2][2];
  }
  Dio_SetPinState(c3PORT, c3, 1);

  return k;
}