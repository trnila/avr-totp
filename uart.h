#ifndef UART_H
#define UART_H
	void uart_init();
	void uart_putchar(uint8_t c);
	uint8_t uart_getch();
	void print(char *msg);
#endif