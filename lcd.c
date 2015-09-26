#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

#define SEND(byte) PORTB = byte
#define CLR_RW() PORTC &= ~(1 << PINC1)
#define CLR_RS() PORTC &= ~(1 << PINC2)
#define SET_RS() PORTC |= (1 << PINC2)

#define SET_E() PORTC |= (1 << PINC0)
#define CLR_E() PORTC &= ~(1 << PINC0)

#define CLOCK() SET_E(); SET_E(); CLR_E(); CLR_E(); _delay_ms(10);

void mylcd_init() {
  DDRB = 0xFF;
  DDRC = (1 << PINC0) | (1 << PINC1) | (1 << PINC2);

  _delay_ms(50);

  // function set
  mylcd_sendcmd(0x30);

  // function set again
  mylcd_sendcmd(0x30);

  // display on/off, cursor
  mylcd_sendcmd(0x0E);

  // display clear
  mylcd_sendcmd(0x01);

  // entry mode set
  mylcd_sendcmd(0x06);

  // cursor home!
  mylcd_sendcmd(0x01);

  // write D!
  //mylcd_senddata('D');
  mylcd_senddata('A');
}

void mylcd_sendcmd(uint8_t cmd) {
  CLR_RS();
  CLR_RW();
  SEND(cmd);
  CLOCK();
}

void mylcd_senddata(uint8_t data) {
  SET_RS();
  CLR_RW();
  SEND(data);
  CLOCK();
}