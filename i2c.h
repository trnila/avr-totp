#ifndef I2C_INCLUDED
	#define I2C_INCLUDED
	#include <stdint.h>
	#include <stdio.h>


#define F_SCL 100000L
#define DS1307 208 // 32 our slave, 160 EEPROM, 208 RTC

#define I2C_DEBUG_PRINT printf

	void I2C_init();
	void I2C_Stop();
	uint8_t I2C_Start(uint8_t addr);
	uint8_t I2C_Write (uint8_t data);
	uint8_t I2C_ReadNACK();
	uint8_t I2C_ReadACK();
	void I2C_WriteRegister(uint8_t deviceRegister, uint8_t data);
	uint8_t I2C_ReadRegister(uint8_t deviceRegister);
#endif