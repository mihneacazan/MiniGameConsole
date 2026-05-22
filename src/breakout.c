#include "breakout.h"
#include "ssd1306.h"
#include "buzzer.h"
#include "pwm.h"
#include "eeprom_scores.h"
#include <util/delay.h>

static void led_set_lives(uint8_t lives, uint8_t start_lives) {
    if (lives == 0) {
        set_rgb_color(0, 0, 0);
    } else if (lives == start_lives) {
        set_rgb_color(0, 200, 0);
    } else if (lives == 1) {
        set_rgb_color(255, 0, 0);
    } else {
        set_rgb_color(255, 120, 0);
    }
}

void breakout_init(BreakoutState* state) {
    state->paddle_x = (SSD1306_WIDTH - PADDLE_WIDTH) / 2;
    state->ball_x = (state->paddle_x + PADDLE_WIDTH / 2 - BALL_SIZE / 2) * 16;
    state->ball_y = (PADDLE_Y - BALL_SIZE - 1) * 16;
    state->ball_vx = 16;
    state->ball_vy = -(int8_t)state->ball_speed;
    
    for (uint8_t r = 0; r < BRICK_ROWS; r++) {
        state->bricks[r] = 0xFF;
    }
    
    state->score = 0;
    state->lives = state->start_lives;
    state->game_over = 0;
    state->game_won = 0;
    state->paused = 0;
    state->pause_selection = 0;
    state->end_selection = 0;
    state->ball_launched = 0;
    state->score_saved = 0;
    
    led_set_lives(state->lives, state->start_lives);
}

void breakout_launch_ball(BreakoutState* state) {
    if (!state->ball_launched) {
        state->ball_launched = 1;
        state->ball_vx = 16;
        state->ball_vy = -(int8_t)state->ball_speed;
    }
}

static void reset_ball(BreakoutState* state) {
    state->ball_x = (state->paddle_x + PADDLE_WIDTH / 2 - BALL_SIZE / 2) * 16;
    state->ball_y = (PADDLE_Y - BALL_SIZE - 1) * 16;
    state->ball_launched = 0;
}

static uint8_t all_bricks_cleared(BreakoutState* state) {
    for (uint8_t r = 0; r < BRICK_ROWS; r++) {
        if (state->bricks[r] != 0) return 0;
    }
    return 1;
}

void breakout_update(BreakoutState* state, uint16_t joy_x) {
    if (state->game_over || state->game_won || state->paused) return;
    
    if (joy_x < 400) {
        uint16_t offset = 400 - joy_x;
        uint8_t speed = (uint16_t)offset * state->paddle_speed / 2000 + 1;
        if (state->paddle_x >= speed) state->paddle_x -= speed;
        else state->paddle_x = 0;
    } else if (joy_x > 624) {
        uint16_t offset = joy_x - 624;
        uint8_t speed = (uint16_t)offset * state->paddle_speed / 2000 + 1;
        if (state->paddle_x + PADDLE_WIDTH + speed <= SSD1306_WIDTH)
            state->paddle_x += speed;
        else state->paddle_x = SSD1306_WIDTH - PADDLE_WIDTH;
    }
    
    if (!state->ball_launched) {
        state->ball_x = (state->paddle_x + PADDLE_WIDTH / 2 - BALL_SIZE / 2) * 16;
        state->ball_y = (PADDLE_Y - BALL_SIZE - 1) * 16;
        return;
    }
    
    state->ball_x += state->ball_vx;
    state->ball_y += state->ball_vy;
    int16_t bx = state->ball_x / 16;
    int16_t by = state->ball_y / 16;
    
    if (bx <= 0) { state->ball_x = 0; state->ball_vx = -state->ball_vx; bx = 0; }
    if (bx + BALL_SIZE >= SSD1306_WIDTH) {
        state->ball_x = (SSD1306_WIDTH - BALL_SIZE) * 16;
        state->ball_vx = -state->ball_vx; bx = SSD1306_WIDTH - BALL_SIZE;
    }
    if (by <= 0) { state->ball_y = 0; state->ball_vy = -state->ball_vy; by = 0; }
    
    // Paddle collision (top + sides)
    if (bx + BALL_SIZE > state->paddle_x && bx < state->paddle_x + PADDLE_WIDTH &&
        by + BALL_SIZE > PADDLE_Y && by < PADDLE_Y + PADDLE_HEIGHT) {
        int16_t prev_by = (state->ball_y - state->ball_vy) / 16;
        int16_t prev_bx = (state->ball_x - state->ball_vx) / 16;
        if (prev_by + BALL_SIZE <= PADDLE_Y && state->ball_vy > 0) {
            state->ball_vy = -state->ball_vy;
            state->ball_y = (PADDLE_Y - BALL_SIZE) * 16;
            int8_t hit = bx - state->paddle_x - PADDLE_WIDTH / 2;
            if (hit < -5) state->ball_vx = -48;
            else if (hit < -2) state->ball_vx = -32;
            else if (hit > 5) state->ball_vx = 48;
            else if (hit > 2) state->ball_vx = 32;
            else state->ball_vx = 16;
        } else {
            if (prev_bx + BALL_SIZE <= state->paddle_x) {
                if (state->ball_vx > 0) state->ball_vx = -state->ball_vx;
                state->ball_x = (state->paddle_x - BALL_SIZE) * 16;
            } else {
                if (state->ball_vx < 0) state->ball_vx = -state->ball_vx;
                state->ball_x = (state->paddle_x + PADDLE_WIDTH) * 16;
            }
        }
    }
    
    // Ball dropped
    if (by > SSD1306_HEIGHT) {
        state->lives--;
        if (state->lives == 0) {
            state->game_over = 1;
            set_rgb_color(255, 0, 0);
            buzzer_play_tone(800); _delay_ms(200);
            set_rgb_color(150, 0, 0);
            buzzer_play_tone(600); _delay_ms(200);
            set_rgb_color(60, 0, 0);
            buzzer_play_tone(400); _delay_ms(200);
            set_rgb_color(15, 0, 0);
            buzzer_play_tone(200); _delay_ms(400);
            set_rgb_color(0, 0, 0);
            buzzer_stop();
            // Auto-save score
            if (!state->score_saved && state->player_name[0]) {
                scores_save(GAME_BREAKOUT, state->player_name, state->score);
                state->score_saved = 1;
            }
        } else {
            set_rgb_color(255, 0, 0);
            buzzer_play_tone(300); _delay_ms(100);
            set_rgb_color(0, 0, 0); _delay_ms(50);
            set_rgb_color(255, 0, 0);
            buzzer_play_tone(200); _delay_ms(100);
            set_rgb_color(0, 0, 0); _delay_ms(50);
            set_rgb_color(255, 0, 0); _delay_ms(100);
            buzzer_stop();
            led_set_lives(state->lives, state->start_lives);
            reset_ball(state);
        }
        return;
    }
    
    // Bricks collision
    for (uint8_t r = 0; r < BRICK_ROWS; r++) {
        for (uint8_t c = 0; c < BRICK_COLS; c++) {
            if (!(state->bricks[r] & (1 << c))) continue;
            uint8_t brick_x = c * (BRICK_WIDTH + BRICK_GAP) + 1;
            uint8_t brick_y = BRICK_START_Y + r * (BRICK_HEIGHT + BRICK_GAP);
            if (bx + BALL_SIZE > brick_x && bx < brick_x + BRICK_WIDTH &&
                by + BALL_SIZE > brick_y && by < brick_y + BRICK_HEIGHT) {
                state->bricks[r] &= ~(1 << c);
                state->score += 10;
                set_rgb_color(255, 255, 255);
                buzzer_play_tone(4000); _delay_ms(20);
                buzzer_stop();
                led_set_lives(state->lives, state->start_lives);
                state->ball_vy = -state->ball_vy;
                if (all_bricks_cleared(state)) {
                    state->game_won = 1;
                    set_rgb_color(255, 0, 0);
                    buzzer_play_tone(523); _delay_ms(150);
                    set_rgb_color(0, 255, 0);
                    buzzer_play_tone(659); _delay_ms(150);
                    set_rgb_color(0, 0, 255);
                    buzzer_play_tone(784); _delay_ms(150);
                    set_rgb_color(255, 255, 255);
                    buzzer_play_tone(1047); _delay_ms(400);
                    buzzer_stop();
                    set_rgb_color(0, 200, 0);
                    // Auto-save score
                    if (!state->score_saved && state->player_name[0]) {
                        scores_save(GAME_BREAKOUT, state->player_name, state->score);
                        state->score_saved = 1;
                    }
                }
                return;
            }
        }
    }
}

// Draws a menu with options and a selector
static void draw_menu(const char* opts[], uint8_t count, uint8_t sel, uint8_t y_start) {
    for (uint8_t i = 0; i < count; i++) {
        uint8_t y = y_start + i * 10;
        ssd1306_draw_string(36, y, opts[i]);
        if (i == sel) ssd1306_draw_char(28, y, '>');
    }
}

void breakout_render(BreakoutState* state) {
    ssd1306_clear();
    
    // ===== VICTORY SCREEN =====
    if (state->game_won) {
        ssd1306_draw_string(19, 0, "CONGRATULATIONS");
        ssd1306_draw_string(40, 12, "SCORE:");
        ssd1306_draw_number(76, 12, state->score);
        const char* opts[] = {"RESTART", "HOME", "EXIT", "SCORES"};
        draw_menu(opts, 4, state->end_selection, 26);
        ssd1306_update();
        return;
    }
    
    // ===== GAME OVER SCREEN =====
    if (state->game_over) {
        ssd1306_draw_string(34, 0, "GAME OVER");
        ssd1306_draw_string(40, 12, "SCORE:");
        ssd1306_draw_number(76, 12, state->score);
        const char* opts[] = {"RESTART", "HOME", "EXIT", "SCORES"};
        draw_menu(opts, 4, state->end_selection, 26);
        ssd1306_update();
        return;
    }
    
    // ===== GAMEPLAY =====
    ssd1306_draw_string(0, 0, "S:");
    ssd1306_draw_number(12, 0, state->score);
    for (uint8_t i = 0; i < state->lives; i++)
        ssd1306_fill_rect(SSD1306_WIDTH - 8 - i * 6, 1, 4, 4, 1);
    
    for (uint8_t r = 0; r < BRICK_ROWS; r++)
        for (uint8_t c = 0; c < BRICK_COLS; c++)
            if (state->bricks[r] & (1 << c)) {
                uint8_t bx = c * (BRICK_WIDTH + BRICK_GAP) + 1;
                uint8_t by = BRICK_START_Y + r * (BRICK_HEIGHT + BRICK_GAP);
                ssd1306_fill_rect(bx, by, BRICK_WIDTH, BRICK_HEIGHT, 1);
            }
    
    ssd1306_fill_rect(state->paddle_x, PADDLE_Y, PADDLE_WIDTH, PADDLE_HEIGHT, 1);
    ssd1306_fill_rect(state->ball_x / 16, state->ball_y / 16, BALL_SIZE, BALL_SIZE, 1);
    
    // ===== PAUSE (4 options) =====
    if (state->paused) {
        ssd1306_fill_rect(16, 6, 96, 54, 0);
        ssd1306_draw_hline(16, 6, 96, 1);
        ssd1306_draw_hline(16, 59, 96, 1);
        ssd1306_draw_vline(16, 6, 54, 1);
        ssd1306_draw_vline(111, 6, 54, 1);
        ssd1306_draw_string(46, 10, "PAUSED");
        const char* opts[] = {"CONTINUE", "RESTART", "HOME", "EXIT"};
        draw_menu(opts, 4, state->pause_selection, 22);
    }
    
    ssd1306_update();
}
