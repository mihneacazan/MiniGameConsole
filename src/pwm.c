#include "pwm.h"
#include <avr/io.h>

void pwm_init(void) {
    // Configure pins as OUTPUT:
    // PD6 (OC0A) - Red, PD5 (OC0B) - Green, PB3 (OC2A) - Blue
    DDRD |= (1 << PD6) | (1 << PD5);
    DDRB |= (1 << PB3);

    // Initially, all LEDs are off (0% duty cycle)
    OCR0A = 0;
    OCR0B = 0;
    OCR2A = 0;

    // --- Timer0 Configuration for PD6 and PD5 ---
    // Fast PWM mode (Mode 3: WGM01=1, WGM00=1)
    // Non-inverting mode on OC0A and OC0B (COM0A1=1, COM0B1=1)
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    // Prescaler 64 (CS01=1, CS00=1) -> PWM frequency = 16MHz / (64 * 256) = ~976 Hz
    TCCR0B = (1 << CS01) | (1 << CS00);

    // --- Timer2 Configuration for PB3 ---
    // Fast PWM mode (Mode 3: WGM21=1, WGM20=1)
    // Non-inverting mode on OC2A (COM2A1=1)
    TCCR2A = (1 << COM2A1) | (1 << WGM21) | (1 << WGM20);
    // Prescaler 64 (CS22=1)
    TCCR2B = (1 << CS22);
}

void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue) {
    // OCR (Output Compare Register) registers dictate the light intensity
    // OCR0A (PD6) is connected to pin 20 (Red)
    OCR0A = red;   
    
    // OCR0B (PD5) is connected to pin 22 (Green)
    OCR0B = green; 
    
    // OCR2A (PB3) is connected to pin 23 (Blue)
    OCR2A = blue;  
}
