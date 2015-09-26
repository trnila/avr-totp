#ifndef DS1307_INCLUDED
	#define DS1307_INCLUDED

	int DS1307_isRunning();
	void DS1307_getDate(uint8_t *day, uint8_t *month, uint16_t *year);
	void DS1307_getTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

	void DS1307_setDate(uint8_t day, uint8_t month, uint16_t year);
	void DS1307_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);

#endif
