#ifndef BREAKOUT_H
#define BREAKOUT_H

#include <stdint.h>
#include "eeprom_scores.h"

// Game element dimensions
#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 3
#define PADDLE_Y 60          // Paddle is near the bottom edge

#define BALL_SIZE 2

#define BRICK_COLS 8
#define BRICK_ROWS 4
#define BRICK_WIDTH 14
#define BRICK_HEIGHT 4
#define BRICK_GAP 2
#define BRICK_START_Y 10     // Bricks start at pixel 10

// Game state structure
typedef struct {
    // Paddle
    uint8_t paddle_x;

    // Ball (position in "fixed point" x16 for smooth movement)
    int16_t ball_x;
    int16_t ball_y;
    int8_t  ball_vx;
    int8_t  ball_vy;

    // Bricks: each bit = 1 brick (4 bytes = 32 bricks)
    uint8_t bricks[BRICK_ROWS];
    
    // Score and state
    uint16_t score;
    uint8_t  lives;
    uint8_t  game_over;
    uint8_t  game_won;
    uint8_t  paused;
    uint8_t  pause_selection;   // 0=CONTINUE, 1=RESTART, 2=HOME, 3=EXIT
    uint8_t  end_selection;     // 0=RESTART, 1=HOME, 2=EXIT (end screen menu)
    uint8_t  ball_launched;
    uint8_t  score_saved;       // 1 = score was already saved in EEPROM
    
    // Settings
    uint8_t  ball_speed;
    uint8_t  paddle_speed;
    uint8_t  start_lives;
    
    // Player
    char     player_name[MAX_NAME_LEN + 1];
} BreakoutState;

// Initialize the game
void breakout_init(BreakoutState* state);

// Update the game state by one tick
void breakout_update(BreakoutState* state, uint16_t joy_x);

// Launch the ball (on first button press)
void breakout_launch_ball(BreakoutState* state);

// Draw the entire frame and send it to the OLED screen
void breakout_render(BreakoutState* state);

#endif // BREAKOUT_H
