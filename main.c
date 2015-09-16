#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

int main() {
	uart_init();
	print("\r\nHello!\r\n");

	DDRB = 0xFF;

	for(;;) {
		uart_putchar('.');
		_delay_ms(100);

		PORTB ^= 0xFF;
	}

	return 1;
}