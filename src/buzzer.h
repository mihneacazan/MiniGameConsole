#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

// Initialize the pin and Timer for the buzzer
void buzzer_init(void);

// Play a tone at the desired frequency (in Hertz)
void buzzer_play_tone(uint16_t freq);

// Stop the sound
void buzzer_stop(void);

#endif // BUZZER_H
