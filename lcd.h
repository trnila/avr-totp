#ifndef LCD_INCLUDED
	#define LCD_INCLUDED
	#include <stdint.h>

	void mylcd_init();
	void mylcd_sendcmd(uint8_t cmd);
	void mylcd_senddata(uint8_t data);

#endif