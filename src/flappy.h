#ifndef FLAPPY_H
#define FLAPPY_H

#include "eeprom_scores.h"
#include <stdint.h>

#define FLAPPY_BIRD_X 20   // Fixed X position of the bird
#define FLAPPY_BIRD_SIZE 4 // Bird size (4x4)
#define FLAPPY_PIPE_W 8    // Pipe width
#define FLAPPY_GAP_SIZE 20 // Gap size between pipes
#define FLAPPY_GROUND_Y 62 // Ground line Y coordinate
#define FLAPPY_NUM_PIPES 3 // Number of active pipes
#define FLAPPY_GRAVITY 5   // Gravity per frame (fixed-point /16)
#define FLAPPY_FLAP -37    // Flap impulse (fixed-point /16)

typedef struct {
  int16_t bird_y;                       // Y position (fixed-point x16)
  int8_t bird_vy;                       // Vertical velocity
  uint8_t pipe_x[FLAPPY_NUM_PIPES];     // X position of each pipe
  uint8_t pipe_gap_y[FLAPPY_NUM_PIPES]; // Center of the gap
  uint16_t score;
  uint8_t game_over;
  uint8_t started; // 0 = wait for first press
  uint8_t paused;
  uint8_t pause_selection; // 0=CONTINUE, 1=RESTART, 2=HOME, 3=EXIT
  uint8_t end_selection;   // 0=RESTART, 1=HOME, 2=EXIT, 3=SCORES
  uint8_t score_saved;
  uint8_t pipe_scored[FLAPPY_NUM_PIPES]; // 1 = already scored
  char player_name[MAX_NAME_LEN + 1];
} FlappyState;

// Initialize the Flappy Bird game
void flappy_init(FlappyState *state);

// Update game logic (one frame)
void flappy_update(FlappyState *state);

// Bird flaps its wings (button press)
void flappy_flap(FlappyState *state);

// Draw the frame on the framebuffer
void flappy_render(FlappyState *state);

#endif // FLAPPY_H
