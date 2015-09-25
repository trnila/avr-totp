#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

int main() {
	uart_init();

	printf("Hello, %s!\r\n", "world");

	unsigned int seconds = 0;
	for(;;) {
		printf("%d\r\n", seconds++);
		_delay_ms(1000);
	}

	return 1;
}