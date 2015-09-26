#include <stdint.h>
#include "DS1307.h"
#include "i2c.h"

#define REG_SECONDS 0x00
#define REG_MINUTES 0x01
#define REG_HOURS 0x02
#define REG_DAY 0x04
#define REG_MONTH 0x05
#define REG_YEAR 0x06

#define CLOCK_DISABLED_BIT (1 << 7)

void DS1307_setDate(uint8_t day, uint8_t month, uint16_t year) {
	I2C_WriteRegister(REG_DAY, (day / 10) << 4 | (day % 10));
	I2C_WriteRegister(REG_MONTH, (month / 10) << 4 | (month % 10));

	year -= 2000;
	I2C_WriteRegister(REG_YEAR, (year / 10) << 4 | (year % 10));
}

void DS1307_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
	I2C_WriteRegister(REG_SECONDS, ((seconds / 10) << 4) | (seconds % 10));
	I2C_WriteRegister(REG_MINUTES, ((minutes / 10) << 4) | (minutes % 10));
	I2C_WriteRegister(REG_HOURS, 0x40 | ((hours / 10) << 4) | (hours % 10));
}

int DS1307_isRunning() {
	return !(I2C_ReadRegister(REG_SECONDS) & CLOCK_DISABLED_BIT);
}

void DS1307_getTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
	uint8_t raw = I2C_ReadRegister(REG_SECONDS) & ~CLOCK_DISABLED_BIT;
	*seconds = ((raw & 0x70) >> 4) * 10 + (raw & 0x0F);

	raw = I2C_ReadRegister(REG_MINUTES);
	*minutes = ((raw & 0x70) >> 4) * 10 + (raw & 0x0F);

	raw = I2C_ReadRegister(REG_HOURS);
	*hours = ((raw & 0x30) >> 4) * 10 + (raw & 0x0F);
}

void DS1307_getDate(uint8_t *day, uint8_t *month, uint16_t *year) {
	uint8_t raw = I2C_ReadRegister(REG_DAY);
	*day = ((raw & 0x70) >> 4) * 10 + (raw & 0x0F);

	raw = I2C_ReadRegister(REG_MONTH);
	*month = ((raw & 0x70) >> 4) * 10 + (raw & 0x0F);

	raw = I2C_ReadRegister(REG_YEAR);
	*year = ((raw & 0xF0) >> 4) * 10 + (raw & 0x0F) + 2000;
}