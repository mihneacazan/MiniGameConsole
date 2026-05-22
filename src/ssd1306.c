#include "ssd1306.h"
#include "i2c.h"
#include "font5x7.h"
#include <string.h>

// This is our graphical "brain": a 128x64 pixel image means 
// exactly 1024 bytes in RAM (because 1 byte stores 8 vertical pixels).
// We modify this buffer quickly in RAM, and then send it all at once to the screen!
static uint8_t buffer[1024];

static void ssd1306_send_command(uint8_t cmd) {
    i2c_start();
    i2c_write(SSD1306_I2C_ADDR); // Address the screen (Write mode)
    i2c_write(0x00);             // Control byte: 0x00 means a Command follows
    i2c_write(cmd);              // The actual command
    i2c_stop();
}

void ssd1306_init(void) {
    // Initialization sequence recommended by the manufacturer for 128x64 screens
    ssd1306_send_command(0xAE); // Display OFF
    ssd1306_send_command(0xD5); // Set display clock divide ratio
    ssd1306_send_command(0x80); 
    ssd1306_send_command(0xA8); // Set multiplex ratio (63 for 64 height)
    ssd1306_send_command(0x3F); 
    ssd1306_send_command(0xD3); // Set display offset
    ssd1306_send_command(0x00); 
    ssd1306_send_command(0x40); // Set start line la 0
    ssd1306_send_command(0x8D); // Charge pump (Very important: turns on the internal 8V boost converter to light up the pixels!)
    ssd1306_send_command(0x14); 
    ssd1306_send_command(0x20); // Memory addressing mode
    ssd1306_send_command(0x00); // 00 = Horizontal addressing mode
    ssd1306_send_command(0xA1); // Segment remap (Flips the image horizontally)
    ssd1306_send_command(0xC8); // COM output scan dir (Flips the image vertically)
    ssd1306_send_command(0xDA); // COM hardware pin configuration
    ssd1306_send_command(0x12); 
    ssd1306_send_command(0x81); // Set contrast
    ssd1306_send_command(0xCF); 
    ssd1306_send_command(0xD9); // Set pre-charge period
    ssd1306_send_command(0xF1); 
    ssd1306_send_command(0xDB); // Set VCOMH deselect level
    ssd1306_send_command(0x40); 
    ssd1306_send_command(0xA4); // Resume to RAM content display
    ssd1306_send_command(0xA6); // Set normal display (0=black, 1=lit)
    ssd1306_send_command(0xAF); // Display ON (Wake up!)
}

void ssd1306_clear(void) {
    memset(buffer, 0, 1024);
}

void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color) {
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;
    
    uint16_t index = x + (y / 8) * SSD1306_WIDTH;
    if (color) {
        buffer[index] |= (1 << (y % 8));
    } else {
        buffer[index] &= ~(1 << (y % 8));
    }
}

void ssd1306_update(void) {
    for (uint8_t page = 0; page < 8; page++) {
        ssd1306_send_command(0xB0 + page);
        ssd1306_send_command(0x00);
        ssd1306_send_command(0x10);
        
        i2c_start();
        i2c_write(SSD1306_I2C_ADDR);
        i2c_write(0x40);
        
        for (uint8_t col = 0; col < 128; col++) {
            i2c_write(buffer[col + page * 128]);
        }
        i2c_stop();
    }
}

void ssd1306_draw_hline(uint8_t x, uint8_t y, uint8_t w, uint8_t color) {
    for (uint8_t i = 0; i < w; i++) {
        ssd1306_draw_pixel(x + i, y, color);
    }
}

void ssd1306_draw_vline(uint8_t x, uint8_t y, uint8_t h, uint8_t color) {
    for (uint8_t i = 0; i < h; i++) {
        ssd1306_draw_pixel(x, y + i, color);
    }
}

void ssd1306_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    // Clamp dimensions to screen bounds
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) return;
    if (x + w > SSD1306_WIDTH)  w = SSD1306_WIDTH - x;
    if (y + h > SSD1306_HEIGHT) h = SSD1306_HEIGHT - y;

    uint8_t y_end = y + h;

    // Process page by page (each page = 8 vertical pixels)
    for (uint8_t page = y / 8; page <= (y_end - 1) / 8; page++) {
        uint8_t page_top = page * 8;
        uint8_t page_bot = page_top + 7;

        // Determine which bits in this page byte are affected
        uint8_t bit_start = (y > page_top) ? y - page_top : 0;
        uint8_t bit_end   = (y_end - 1 < page_bot) ? y_end - 1 - page_top : 7;

        // Build the bitmask for the affected bits
        uint8_t mask = 0;
        for (uint8_t b = bit_start; b <= bit_end; b++) {
            mask |= (1 << b);
        }

        uint16_t base = (uint16_t)page * SSD1306_WIDTH;
        for (uint8_t col = x; col < x + w; col++) {
            if (color) {
                buffer[base + col] |= mask;
            } else {
                buffer[base + col] &= ~mask;
            }
        }
    }
}

void ssd1306_draw_char(uint8_t x, uint8_t y, char c) {
    // Our font starts from ASCII 32 (space) and goes up to 90 (Z)
    if (c < 32 || c > 90) return;
    
    uint16_t offset = (c - 32) * 5; // 5 bytes per character
    
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t column_data = pgm_read_byte(&font5x7[offset + col]);
        for (uint8_t row = 0; row < 7; row++) {
            if (column_data & (1 << row)) {
                ssd1306_draw_pixel(x + col, y + row, 1);
            }
        }
    }
}

void ssd1306_draw_string(uint8_t x, uint8_t y, const char* str) {
    while (*str) {
        ssd1306_draw_char(x, y, *str);
        x += 6; // 5 pixels character + 1 pixel space
        str++;
    }
}

void ssd1306_draw_number(uint8_t x, uint8_t y, uint16_t num) {
    char buf[6]; // Max 5 digits + null
    uint8_t i = 0;
    
    if (num == 0) {
        ssd1306_draw_char(x, y, '0');
        return;
    }
    
    // Build the string in reverse
    while (num > 0 && i < 5) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // Display it from tail to head
    for (uint8_t j = 0; j < i; j++) {
        ssd1306_draw_char(x + j * 6, y, buf[i - 1 - j]);
    }
}

