#include "buzzer.h"
#include <avr/io.h>

void buzzer_init(void) {
    // PB1 (OC1A) starts as INPUT (high-impedance) — buzzer is completely disconnected
    DDRB &= ~(1 << PB1);
    PORTB &= ~(1 << PB1);  // No pull-up
    
    // Timer1 completely stopped
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 0;
}

void buzzer_play_tone(uint16_t freq) {
    if (freq == 0) {
        buzzer_stop();
        return;
    }
    
    // 1. Configure PB1 as OUTPUT (reconnect the pin)
    DDRB |= (1 << PB1);
    
    // 2. Stop the timer completely before reconfiguration
    TCCR1B = 0;
    TCCR1A = 0;
    
    // 3. Set the desired frequency
    // Formula: OCR1A = F_CPU / (2 * Prescaler * freq) - 1
    // With prescaler 64: OCR1A = 125000 / freq - 1
    uint16_t ocr_value = (125000UL / freq) - 1;
    OCR1A = ocr_value;
    
    // 4. Reset counter to 0
    TCNT1 = 0;
    
    // 5. Configure Timer1: CTC + Toggle OC1A on compare match
    TCCR1A = (1 << COM1A0);               // Toggle OC1A on match
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);  // CTC + prescaler 64
}

void buzzer_stop(void) {
    // 1. Stop Timer1 completely (all registers to 0)
    TCCR1B = 0;   // Stop the clock FIRST (prevents any toggle during deactivation)
    TCCR1A = 0;   // Disconnect OC1A from the pin
    TCNT1 = 0;    // Reset the counter
    OCR1A = 0;    // Reset the compare value
    
    // 2. Force PB1 LOW, then set it as INPUT (high-impedance)
    // This completely disconnects the buzzer from any driver
    PORTB &= ~(1 << PB1);  // LOW first
    DDRB &= ~(1 << PB1);   // Then INPUT — the pin becomes high-impedance
}
