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

#define CMD_CLEAR		0x01
#define CMD_GOTO_HOME	0x02

long generate(uint32_t seed) {
	//uint8_t secret[] = "YSTE2NSM5QB6OMZE";
	uint8_t key[] = { 0xC4, 0xA6, 0x4D, 0x36, 0x4C, 0xEC, 0x03, 0xE7, 0x33, 0x24};

	//long long keylen = base32_decode(secret, key, sizeof(key));

	unsigned long long message = (unsigned long long)(seed);// / 30;

	uint8_t val[8];
	for (long i = 8; i--; message >>= 8) {
		val[i] = message;
	}


	uint8_t hash[20];
	hmac_sha1(hash, key, sizeof(key)/sizeof(key[0])*8, val, 8*8);


	long long offset = hash[19/**/] & 0xF;
	unsigned long long truncatedHash = 0;
	for (long i = 0; i < 4; ++i) {
		truncatedHash <<= 8;
		truncatedHash  |= hash[offset + i];
	}
	memset(hash, 0, sizeof(hash));
	truncatedHash &= 0x7FFFFFFF;
	truncatedHash %= 1000000;
	return truncatedHash;
}


int main() {
	uart_init();
	I2C_init();

	printf("Hello, world!\r\n");

	uint8_t hash[20] = {0};
	hmac_sha1(hash, "key", 3*8, "hello", 5*8);
	//sha1(hash, "hello", 5*8);
	for(int i = 0; i < 20; i++) {
		printf("%02X", hash[i]);
	}

	printf("\n");

	//return 0;

	//printf("%d", gencode());

	mylcd_init();

	//unsigned int code = generate();
	unsigned long code = generate(TIME/30);
	long d = 100000;
	while(code > 0) {
		mylcd_senddata((code / d) + '0');
		code -= (code / d) * d;
		d /= 10;
	}

	_delay_ms(2000);

	#define BCD(digit) ((digit) - '0')

	// set time
	I2C_Start(DS1307);
	I2C_Write(0x00); // set register pointer
	I2C_Write((BCD(__TIME__[6]) << 4) | BCD(__TIME__[7])); // start timer and set seconds
	I2C_Write((BCD(__TIME__[3]) << 4) | BCD(__TIME__[4])); // minutes
	I2C_Write(0x40 | (BCD(__TIME__[0]) << 4) | BCD(__TIME__[1])); // hours
	I2C_Write(BCD(__DATE__[4]) << 4 | BCD(__DATE__[5])); // day

	I2C_Stop();

	for(;;) {
		uint8_t raw = I2C_ReadRegister(0x00) & ~(1 << 7);
		int seconds = ((raw & 0x70) >> 4) * 10 + (raw & 0x0F);

		raw = I2C_ReadRegister(0x01);
		int minutes = ((raw & 0x70) >> 4) * 10 + (raw & 0x0F);

		raw = I2C_ReadRegister(0x02);
		int hours = ((raw & 0x30) >> 4) * 10 + (raw & 0x0F);

		raw = I2C_ReadRegister(0x03);
		int day = (raw & 0x30 >> 4) * 10 + (raw & 0x0F);
		int month = 9;
		int year = 2015;

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

		mylcd_sendcmd(0x01);
		unsigned long code = generate(seed);
		long d = 100000;
		while(code > 0) {
			mylcd_senddata((code / d) + '0');
			code -= (code / d) * d;
			d /= 10;
		}

		_delay_ms(1000);
	}

	for(;;);





	return 1;
}