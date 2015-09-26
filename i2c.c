#include <util/twi.h>
#include "i2c.h"

void I2C_init() {
	TWSR = 0;
	TWBR = ((F_CPU / F_SCL) - 16) / 2;
}

void I2C_Stop() {
	// send stop condition
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

uint8_t I2C_Start(uint8_t addr) {
	// send start condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait until start condition is sent
	while (!(TWCR & (1 << TWINT)));

	// check if start has been successfull
	if(TW_STATUS != TW_START && TW_STATUS != TW_REP_START) {
		I2C_DEBUG_PRINT("\nI2C_start: TW_START not received, got: %X\r\n", TW_STATUS);
		return 0;
	}

	// start condition has been sent and we have to sent address now
	// load slave address to data register
	TWDR = addr;

	// send address
	TWCR = (1 << TWINT) | (1 << TWEN);

	// wait untill address is sent
	while (!(TWCR & (1 << TWINT)));

	// check if slave have acknowledged address
	if(TW_STATUS != TW_MT_SLA_ACK && TW_STATUS != TW_MR_SLA_ACK) {
		I2C_DEBUG_PRINT("\nI2C_start: TW_Mx_SLA_ACK not received, got: %X\r\n", TW_STATUS);
		return 0;
	}

	return 1;
}

uint8_t I2C_Write (uint8_t data) {
	// load data to data register
	TWDR = data;

	// send them
	TWCR = (1 << TWINT) | (1 << TWEN);

	// wait for transmission
	while (!(TWCR & (1 << TWINT)));

	// check if we have received ACK bit
	if(TW_STATUS != TW_MT_DATA_ACK) {
		I2C_DEBUG_PRINT("\nI2C_write: TW_MT_DATA_ACK not received, got: %X\r\n", TW_STATUS);
		return 0;
	}
	return 1;
}

uint8_t I2C_ReadNACK() {
	TWCR = (1 << TWINT) | (1 << TWEN);

	// wait untill we receive data
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

uint8_t I2C_ReadACK() {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

	// wait untill we receive data
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}


void I2C_WriteRegister(uint8_t deviceRegister, uint8_t data) {
	I2C_Start(DS1307);

	// send address
	I2C_Write(deviceRegister);

	// write data to that address
	I2C_Write(data);
	I2C_Stop();
}
uint8_t I2C_ReadRegister(uint8_t deviceRegister)
{
	uint8_t data = 0;

	// start and write address
	I2C_Start(DS1307);
	I2C_Write(deviceRegister); // set register pointer

	// repeated start for read from address we sent earlier
	I2C_Start(DS1307 + TW_READ);
	data = I2C_ReadNACK();
	I2C_Stop();
	return data;
}