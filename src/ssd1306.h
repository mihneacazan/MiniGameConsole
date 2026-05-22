#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

// Screen resolution
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

// Hardware I2C address of the screen (usually 0x3C, shifted becomes 0x78)
#define SSD1306_I2C_ADDR 0x78

// Initializes the OLED screen
void ssd1306_init(void);

// Clears the internal memory (framebuffer) with 0 (black)
void ssd1306_clear(void);

// Draws a pixel on the screen at coordinates x, y. 
// color: 1 = white, 0 = black
void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color);

// Sends the entire framebuffer from the ATmega's RAM to the OLED
void ssd1306_update(void);

// Draws a horizontal line
void ssd1306_draw_hline(uint8_t x, uint8_t y, uint8_t w, uint8_t color);

// Draws a vertical line
void ssd1306_draw_vline(uint8_t x, uint8_t y, uint8_t h, uint8_t color);

// Draws a filled rectangle
void ssd1306_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);

// Draws a single character at position (x, y) — 5x7 font
void ssd1306_draw_char(uint8_t x, uint8_t y, char c);

// Draws a string (text) at position (x, y)
void ssd1306_draw_string(uint8_t x, uint8_t y, const char* str);

// Draws a number (uint16) at position (x, y)
void ssd1306_draw_number(uint8_t x, uint8_t y, uint16_t num);

#endif // SSD1306_H
