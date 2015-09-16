#include <avr/io.h>
#include <util/setbaud.h>
#include "uart.h"

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
}

void uart_putchar(uint8_t c) {
	while ( !( UCSRA & (1<<UDRE)) );
	UDR = c;
}

uint8_t uart_getch() {
	while(!(UCSRA & (1<<RXC)));
	return UDR;
}

void print(char *msg) {
	for(; *msg != '\0'; msg++) {
		uart_putchar(*msg);
	}
}