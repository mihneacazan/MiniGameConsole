#ifndef ADC_H
#define ADC_H

#include <stdint.h>

// Initialize the Analog-to-Digital Converter (ADC)
void adc_init(void);

// Read the analog value (0 - 1023) from a specific channel (e.g. 0 for PC0, 1 for PC1)
uint16_t adc_read(uint8_t channel);

#endif // ADC_H
