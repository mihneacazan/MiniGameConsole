#include "i2c.h"
#include <avr/io.h>

void i2c_init(void) {
    // Configure TWI (I2C) hardware for 400 kHz (Fast Mode).
    // Formula from datasheet: SCL_freq = F_CPU / (16 + 2 * TWBR * Prescaler)
    // 400,000 = 16,000,000 / (16 + 2 * 12 * 1) = 16M / 40 = 400k
    
    TWSR = 0x00; // Prescaler = 1
    TWBR = 12;   // Calculated value
    TWCR = (1 << TWEN); // Enable TWI module
}

// Short and fast timeout: uint16_t on 8-bit AVR is native (2 registers)
// 5000 iterations * ~6 cycles = 30000 cycles = ~2ms at 16MHz
// An I2C operation at 400kHz takes max 25us, so 2ms is more than enough.
static inline uint8_t i2c_wait(void) {
    uint16_t timeout = 5000;
    while (!(TWCR & (1 << TWINT))) {
        if (--timeout == 0) {
            TWCR = 0;
            TWCR = (1 << TWEN);
            return 0;
        }
    }
    return 1;
}

void i2c_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    i2c_wait();
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    i2c_wait();
}
