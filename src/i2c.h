#ifndef I2C_H
#define I2C_H

#include <stdint.h>

// Initialize the TWI (Two-Wire Interface) hardware at 400kHz
void i2c_init(void);

// Generate a START condition on the I2C bus
void i2c_start(void);

// Generate a STOP condition on the I2C bus
void i2c_stop(void);

// Send a byte (8 bits) on the I2C bus
void i2c_write(uint8_t data);

#endif // I2C_H
