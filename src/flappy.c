#include "flappy.h"
#include "ssd1306.h"
#include "buzzer.h"
#include "pwm.h"
#include "eeprom_scores.h"
#include "adc.h"
#include <util/delay.h>
#include <stdlib.h>

// Simple PRNG based on LFSR (seed from ADC noise)
static uint16_t rng_state = 1;

static void rng_seed(void) {
    // Read an unconnected ADC channel for noise
    rng_state = adc_read(2) ^ (adc_read(3) << 4) ^ 0xACE1;
    if (rng_state == 0) rng_state = 1;
}

static uint8_t rng_next(uint8_t min, uint8_t max) {
    // 16-bit LFSR
    uint8_t bit = ((rng_state >> 0) ^ (rng_state >> 2) ^
                   (rng_state >> 3) ^ (rng_state >> 5)) & 1;
    rng_state = (rng_state >> 1) | (bit << 15);
    return min + (rng_state % (max - min + 1));
}

static void flappy_led_update(uint16_t score) {
    if (score >= 15) set_rgb_color(255, 0, 0);
    else if (score >= 5) set_rgb_color(255, 180, 0);
    else set_rgb_color(0, 200, 0);
}

void flappy_init(FlappyState* state) {
    rng_seed();
    state->bird_y = 25 * 16;  // Centrul ecranului, fixed-point
    state->bird_vy = 0;
    state->score = 0;
    state->game_over = 0;
    state->started = 0;
    state->paused = 0;
    state->pause_selection = 0;
    state->end_selection = 0;
    state->score_saved = 0;

    // Initialize pipes at equal distances, off-screen
    for (uint8_t i = 0; i < FLAPPY_NUM_PIPES; i++) {
        state->pipe_x[i] = 128 + i * 45;  // Spaced at 45px
        state->pipe_gap_y[i] = rng_next(15, FLAPPY_GROUND_Y - 15);
        state->pipe_scored[i] = 0;
    }
    
    set_rgb_color(0, 200, 0);
}

void flappy_flap(FlappyState* state) {
    if (state->game_over || state->paused) return;
    if (!state->started) state->started = 1;
    state->bird_vy = FLAPPY_FLAP;
    buzzer_play_tone(2000);
    _delay_ms(15);
    buzzer_stop();
}

// Calculates the current gap based on score (shrinks by 1px every 5 points, min 14)
static uint8_t current_gap(uint16_t score) {
    uint8_t shrink = score / 5;
    if (shrink > 6) shrink = 6;  // 20 - 6 = 14 minimum
    return FLAPPY_GAP_SIZE - shrink;
}

// Calculates scroll speed based on score
static uint8_t current_speed(uint16_t score) {
    if (score >= 30) return 4;
    if (score >= 15) return 3;
    return 2;
}

void flappy_update(FlappyState* state) {
    if (state->game_over || state->paused || !state->started) return;
    
    uint8_t gap = current_gap(state->score);
    uint8_t spd = current_speed(state->score);
    
    // Gravity
    state->bird_vy += FLAPPY_GRAVITY;
    if (state->bird_vy > 40) state->bird_vy = 40;
    state->bird_y += state->bird_vy;
    
    uint8_t by = state->bird_y / 16;
    
    if (state->bird_y < 0) { state->bird_y = 0; state->bird_vy = 0; }
    if (by + FLAPPY_BIRD_SIZE >= FLAPPY_GROUND_Y) {
        state->game_over = 1;
    }
    
    // Pipe scrolling + collisions
    for (uint8_t i = 0; i < FLAPPY_NUM_PIPES; i++) {
        if (state->pipe_x[i] > 0 && state->pipe_x[i] < 250) {
            state->pipe_x[i] -= spd;
        } else {
            uint8_t max_x = 0;
            for (uint8_t j = 0; j < FLAPPY_NUM_PIPES; j++) {
                if (state->pipe_x[j] > max_x && state->pipe_x[j] < 250)
                    max_x = state->pipe_x[j];
            }
            state->pipe_x[i] = (max_x > 80) ? max_x + 45 : 128;
            state->pipe_gap_y[i] = rng_next(gap/2 + 2, FLAPPY_GROUND_Y - gap/2 - 2);
            state->pipe_scored[i] = 0;
        }
        
        // Scoring — bird passed the pipe
        if (!state->pipe_scored[i] && 
            state->pipe_x[i] + FLAPPY_PIPE_W < FLAPPY_BIRD_X) {
            state->pipe_scored[i] = 1;
            state->score++;
            // Short white LED flash
            set_rgb_color(255, 255, 255);
            buzzer_play_tone(3000);
            _delay_ms(15);
            buzzer_stop();
            flappy_led_update(state->score);
        }
        
        // Collision with the pipe
        if (state->pipe_x[i] < 250) {
            uint8_t px = state->pipe_x[i];
            uint8_t gap_top = state->pipe_gap_y[i] - gap / 2;
            uint8_t gap_bot = state->pipe_gap_y[i] + gap / 2;
            if (FLAPPY_BIRD_X + FLAPPY_BIRD_SIZE > px && 
                FLAPPY_BIRD_X < px + FLAPPY_PIPE_W) {
                // Check if the bird hits the front wall or is already inside the tunnel
                if (FLAPPY_BIRD_X < px) {
                    // Bird enters the pipe (front wall) with 2px tolerance
                    if (by + 2 < gap_top || by + FLAPPY_BIRD_SIZE - 2 > gap_bot) {
                        state->game_over = 1;
                    }
                } else {
                    // Bird is inside the tunnel — slides on ceiling / floor
                    if (by + FLAPPY_BIRD_SIZE > gap_bot) {
                        state->bird_y = (gap_bot - FLAPPY_BIRD_SIZE) * 16;
                        state->bird_vy = 0;
                    } else if (by < gap_top) {
                        state->bird_y = gap_top * 16;
                        state->bird_vy = 0;
                    }
                }
            }
        }
    }
    
    // Game over logic
    if (state->game_over) {
        // Descending melody + LED fade
        set_rgb_color(255, 0, 0);
        buzzer_play_tone(600); _delay_ms(150);
        set_rgb_color(120, 0, 0);
        buzzer_play_tone(400); _delay_ms(150);
        set_rgb_color(40, 0, 0);
        buzzer_play_tone(200); _delay_ms(300);
        set_rgb_color(0, 0, 0);
        buzzer_stop();
        
        if (!state->score_saved && state->player_name[0]) {
            scores_save(GAME_FLAPPY, state->player_name, state->score);
            state->score_saved = 1;
        }
    }
}

// Helper: draws the options menu (shared with breakout)
static void flappy_draw_menu(const char* opts[], uint8_t count, uint8_t sel, uint8_t y_start) {
    for (uint8_t i = 0; i < count; i++) {
        uint8_t y = y_start + i * 10;
        ssd1306_draw_string(36, y, opts[i]);
        if (i == sel) ssd1306_draw_char(28, y, '>');
    }
}

void flappy_render(FlappyState* state) {
    ssd1306_clear();
    
    // Game Over screen
    if (state->game_over) {
        ssd1306_draw_string(34, 0, "GAME OVER");
        ssd1306_draw_string(40, 12, "SCORE:");
        ssd1306_draw_number(76, 12, state->score);
        const char* opts[] = {"RESTART", "HOME", "EXIT", "SCORES"};
        flappy_draw_menu(opts, 4, state->end_selection, 26);
        ssd1306_update();
        return;
    }
    
    // Ground line
    ssd1306_draw_hline(0, FLAPPY_GROUND_Y, 128, 1);
    
    // Pipes
    for (uint8_t i = 0; i < FLAPPY_NUM_PIPES; i++) {
        uint8_t px = state->pipe_x[i];
        if (px >= 128 || px > 240) continue; // Off-screen
        
        uint8_t gap = current_gap(state->score);
        uint8_t gap_top = state->pipe_gap_y[i] - gap / 2;
        uint8_t gap_bot = state->pipe_gap_y[i] + gap / 2;
        
        // Top pipe
        if (gap_top > 0) {
            uint8_t w = FLAPPY_PIPE_W;
            if (px + w > 128) w = 128 - px;
            ssd1306_fill_rect(px, 0, w, gap_top, 1);
        }
        // Bottom pipe
        if (gap_bot < FLAPPY_GROUND_Y) {
            uint8_t w = FLAPPY_PIPE_W;
            if (px + w > 128) w = 128 - px;
            ssd1306_fill_rect(px, gap_bot, w, FLAPPY_GROUND_Y - gap_bot, 1);
        }
    }
    
    // Bird
    uint8_t by = state->bird_y / 16;
    if (by > 60) by = 60;
    ssd1306_fill_rect(FLAPPY_BIRD_X, by, FLAPPY_BIRD_SIZE, FLAPPY_BIRD_SIZE, 1);
    
    // Score
    ssd1306_draw_string(0, 0, "S:");
    ssd1306_draw_number(12, 0, state->score);
    
    // Start screen
    if (!state->started) {
        ssd1306_draw_string(28, 20, "FLAPPY BIRD");
        ssd1306_draw_string(22, 36, "PRESS TO FLAP");
    }
    
    // Pause menu
    if (state->paused) {
        ssd1306_fill_rect(16, 6, 96, 54, 0);
        ssd1306_draw_hline(16, 6, 96, 1);
        ssd1306_draw_hline(16, 59, 96, 1);
        ssd1306_draw_vline(16, 6, 54, 1);
        ssd1306_draw_vline(111, 6, 54, 1);
        ssd1306_draw_string(46, 10, "PAUSED");
        const char* opts[] = {"CONTINUE", "RESTART", "HOME", "EXIT"};
        flappy_draw_menu(opts, 4, state->pause_selection, 22);
    }
    
    ssd1306_update();
}
