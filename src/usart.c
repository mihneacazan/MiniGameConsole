#include "usart.h"
#include <avr/io.h>
#include <stdlib.h>

void usart_init(uint32_t baud) {
    // Standard formula: (F_CPU / (16 * Baud)) - 1
    // At 9600 baud and 16MHz, UBRR = 103 (0.2% error, practically perfect)
    uint16_t ubrr_value = (16000000UL / (16UL * baud)) - 1;
    
    UBRR0H = (uint8_t)(ubrr_value >> 8);
    UBRR0L = (uint8_t)ubrr_value;
    
    // Enable Transmission (TX)
    UCSR0B = (1 << TXEN0);
    
    // Format: 8 data bits, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void usart_print_char(char c) {
    // Wait until the transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));
    // Put the character in the data register to be sent
    UDR0 = c;
}

void usart_print_string(const char* s) {
    while (*s) {
        usart_print_char(*s++);
    }
}

void usart_print_number(uint16_t n) {
    char buffer[7];
    // Convert the number to text (base 10)
    utoa(n, buffer, 10);
    usart_print_string(buffer);
}
