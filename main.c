#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "uart.h"
#include "lcd.h"
#include "hmac-sha1.h"
#include "base32.h"
#include "i2c.h"
#include "DS1307.h"

uint32_t generate(uint32_t seed) {
	//uint8_t secret[] = "YSTE2NSM5QB6OMZE";
	uint8_t key[] = { 0xC4, 0xA6, 0x4D, 0x36, 0x4C, 0xEC, 0x03, 0xE7, 0x33, 0x24};

	uint8_t val[8];
	for (long i = 8; i--; seed >>= 8) {
		val[i] = seed;
	}

	uint8_t hash[20];
	hmac_sha1(hash, key, sizeof(key) / sizeof(key[0]) * 8, val, 8 * 8);

	uint32_t offset = hash[19] & 0xF;
	uint32_t truncatedHash = 0;
	for (int i = 0; i < 4; ++i) {
		truncatedHash <<= 8;
		truncatedHash  |= hash[offset + i];
	}
	truncatedHash &= 0x7FFFFFFF;
	truncatedHash %= 1000000;
	return truncatedHash;
}


int parseMonth() {
	char months[][3] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	for(int i = 0; i < sizeof(months) / sizeof(months[0]); i++) {
		if(strncmp(months[i], __DATE__, 3) == 0) {
			return i + 1;
		}
	}

	return 1;
}

int main() {
	uart_init();
	printf("\r\n\r\nUART initialized...\r\n");
	I2C_init();
	printf("I2C initialized...\r\n");
	mylcd_init();
	printf("LCD initialized...\r\n");

	printf("DS1307: %s\r\n", DS1307_isRunning() ? "is running " : "IS NOT RUNNING");

	#define CHAR_TO_INT(val) ((val) == ' ' ? 0 :((val) - '0'))
	#define PARSE(tens, num) (CHAR_TO_INT((tens)) * 10 + CHAR_TO_INT((num)))

	DS1307_setTime(PARSE(__TIME__[0], __TIME__[1]), PARSE(__TIME__[3], __TIME__[4]), PARSE(__TIME__[6], __TIME__[7]) + 10);
	DS1307_setDate(PARSE(__DATE__[4], __DATE__[5]), parseMonth(), PARSE(__DATE__[7], __DATE__[8]) * 100 + PARSE(__DATE__[9], __DATE__[10]));

	uint8_t hours, minutes, seconds;
	uint8_t day, month;
	uint16_t year;
	DS1307_getTime(&hours, &minutes, &seconds);
	DS1307_getDate(&day, &month, &year);
	printf("DS1307: Current time %d. %d. %d %02d:%02d:%02d\r\n", day, month, year, hours, minutes, seconds);

	for(;;) {
		DS1307_getTime(&hours, &minutes, &seconds);
		DS1307_getDate(&day, &month, &year);

		struct tm timeinfo;
		memset(&timeinfo, 0, sizeof(timeinfo));
		timeinfo.tm_year = year - 1900;
		timeinfo.tm_mon = month - 1;
		timeinfo.tm_mday = day;
		timeinfo.tm_hour = hours;
		timeinfo.tm_min = minutes;
		timeinfo.tm_sec = seconds;

		uint32_t seed = 31556160 - 240 + mktime(&timeinfo) / 30;

		printf("Read: %d:%d:%d %lu %d.%d.%d %lu\r\n", hours, minutes, seconds, mktime(&timeinfo), day, month, year, seed);

		mylcd_sendcmd(0x80);
		uint32_t code = generate(seed);
		uint32_t d = 100000;
		while(code > 0) {
			mylcd_senddata((code / d) + '0');
			code -= (code / d) * d;
			d /= 10;
		}

		_delay_ms(1000);
	}

	return 1;
}