#include <avr/io.h>
#include <util/setbaud.h>
#include <stdio.h>
#include "uart.h"

int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void uart_init() {
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	#if USE_2X
		UCSRA |= (1 << U2X);
	#else
		UCSRA &= ~(1 << U2X);
	#endif

	UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRC = (1<<URSEL)|(3<<UCSZ0);

	stdout = &mystdout;
}

int uart_putchar(char c, FILE *stream) {
	while ( !( UCSRA & (1<<UDRE)) );
	UDR = c;
	return 0;
}

uint8_t uart_getch() {
	while(!(UCSRA & (1<<RXC)));
	return UDR;
}