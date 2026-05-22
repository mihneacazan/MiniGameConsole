#ifndef USART_H
#define USART_H

#include <stdint.h>

// Initialize serial communication at a specific baud rate
void usart_init(uint32_t baud);

// Send a single character
void usart_print_char(char c);

// Send a string of characters (text)
void usart_print_string(const char* s);

// Send an integer (useful for ADC values)
void usart_print_number(uint16_t n);

#endif // USART_H
