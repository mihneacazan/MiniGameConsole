#include "adc.h"
#include <avr/io.h>

void adc_init(void) {
    // Set the reference voltage to AVCC (5V on our board)
    ADMUX = (1 << REFS0);
    
    // Enable the ADC and set the prescaler to 128 (16MHz / 128 = 125 KHz)
    // The ideal frequency for the ATmega ADC is between 50-200 KHz
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(uint8_t channel) {
    // Select the desired channel (0 for PC0, 1 for PC1, etc.)
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    
    // "Dummy" read - the first conversion after changing the channel is inaccurate
    // because the internal capacitor still holds the previous channel's voltage
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    
    // Average 4 consecutive readings to filter out noise
    uint16_t sum = 0;
    for (uint8_t i = 0; i < 4; i++) {
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));
        sum += ADC;
    }
    
    return sum / 4;
}
