#ifndef PWM_H
#define PWM_H

#include <stdint.h>

// Initialize hardware timers (Timer0 and Timer2) to generate PWM signals
void pwm_init(void);

// Set the RGB LED color. Values can be between 0 (off) and 255 (maximum brightness).
void set_rgb_color(uint8_t red, uint8_t green, uint8_t blue);

#endif // PWM_H
