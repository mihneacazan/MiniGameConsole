#include "adc.h"
#include "breakout.h"
#include "buzzer.h"
#include "eeprom_scores.h"
#include "flappy.h"
#include "i2c.h"
#include "pwm.h"
#include "ssd1306.h"
#include "usart.h"
#include <avr/io.h>
#include <util/delay.h>

// ============================================================
// CHANGE THIS VALUE TO SELECT THE DESIRED TEST:
//   1 = RGB LED Test (color cycle)
//   3 = Buzzer Test (plays a musical scale)
//   4 = OLED Test (Draws geometric shapes)
//   5 = Buttons Test (START/PAUSE)
//   6 = MINI GAME CONSOLE
// ============================================================
#define TEST_TASK 6
// Joystick Pins
#define JOY_X_CHANNEL 0
#define JOY_Y_CHANNEL 1
#define JOY_SW_PIN PD2

// Additional Button Pins
#define BTN_BACK PD3
#define BTN_HOME PB0

// ==================== TEST 1: RGB LED ====================
void test_led(void) {
  pwm_init();
  usart_init(9600);
  usart_print_string("=== TEST 1: RGB LED ===\n");
  usart_print_string(
      "You should see: RED -> GREEN -> BLUE -> WHITE -> OFF\n\n");

  while (1) {
    usart_print_string("RED\n");
    set_rgb_color(255, 0, 0);
    _delay_ms(1000);

    usart_print_string("GREEN\n");
    set_rgb_color(0, 255, 0);
    _delay_ms(1000);

    usart_print_string("BLUE\n");
    set_rgb_color(0, 0, 255);
    _delay_ms(1000);

    usart_print_string("WHITE (all max)\n");
    set_rgb_color(255, 255, 255);
    _delay_ms(1000);

    usart_print_string("OFF\n\n");
    set_rgb_color(0, 0, 0);
    _delay_ms(1000);
  }
}

// ==================== TEST 2: JOYSTICK ====================
void test_joystick(void) {
  pwm_init();
  adc_init();
  usart_init(9600);

  // SW button as input with pull-up
  DDRD &= ~(1 << JOY_SW_PIN);
  PORTD |= (1 << JOY_SW_PIN);

  usart_print_string("=== TEST 2: JOYSTICK ===\n");
  usart_print_string("Move the joystick and watch the values.\n");
  usart_print_string("Center should be ~512 on both axes.\n\n");

  while (1) {
    uint16_t x_val = adc_read(JOY_X_CHANNEL);
    uint16_t y_val = adc_read(JOY_Y_CHANNEL);
    uint8_t btn = !(PIND & (1 << JOY_SW_PIN));

    uint8_t r = x_val / 4;
    uint8_t b = y_val / 4;
    uint8_t g = 0;

    if (btn) {
      r = 0;
      b = 0;
      g = 255;
    }

    set_rgb_color(r, g, b);

    usart_print_string("X: ");
    usart_print_number(x_val);
    usart_print_string(" | Y: ");
    usart_print_number(y_val);
    usart_print_string(" | R: ");
    usart_print_number(r);
    usart_print_string(" | G: ");
    usart_print_number(g);
    usart_print_string(" | B: ");
    usart_print_number(b);
    if (btn)
      usart_print_string(" [CLICK!]");
    usart_print_string("\n");

    _delay_ms(100);
  }
}

// ==================== TEST 3: BUZZER ====================
void test_buzzer(void) {
  buzzer_init();
  usart_init(9600);
  usart_print_string("=== TEST 3: BUZZER ===\n");
  usart_print_string(
      "You should hear a musical scale (Do Re Mi Fa Sol La Si Do)\n\n");

  // Musical note frequencies (4th octave)
  uint16_t note[] = {262, 294, 330, 349, 392, 440, 494, 523};
  const char *nume[] = {"Do", "Re", "Mi", "Fa", "Sol", "La", "Si", "Do+"};

  while (1) {
    for (uint8_t i = 0; i < 8; i++) {
      usart_print_string(nume[i]);
      usart_print_string(" (");
      usart_print_number(note[i]);
      usart_print_string(" Hz)\n");

      buzzer_play_tone(note[i]);
      _delay_ms(500);
      buzzer_stop();
      _delay_ms(100);
    }

    usart_print_string("--- 2 Seconds Pause ---\n\n");
    _delay_ms(2000);
  }
}

// ==================== TEST 4: OLED ====================
void test_oled(void) {
  i2c_init();
  ssd1306_init();

  ssd1306_clear();

  for (uint8_t x = 0; x < SSD1306_WIDTH; x++) {
    ssd1306_draw_pixel(x, 0, 1);
    ssd1306_draw_pixel(x, SSD1306_HEIGHT - 1, 1);
  }
  for (uint8_t y = 0; y < SSD1306_HEIGHT; y++) {
    ssd1306_draw_pixel(0, y, 1);
    ssd1306_draw_pixel(SSD1306_WIDTH - 1, y, 1);
  }

  for (uint8_t i = 0; i < 64; i++) {
    ssd1306_draw_pixel(i * 2, i, 1);
    ssd1306_draw_pixel(127 - (i * 2), i, 1);
  }

  ssd1306_update();

  while (1) {
  }
}

// ==================== TEST 5: BUTTONS ====================
void test_buttons(void) {
  usart_init(9600);

  DDRD &= ~((1 << JOY_SW_PIN) | (1 << BTN_BACK));
  PORTD |= (1 << JOY_SW_PIN) | (1 << BTN_BACK);

  DDRB &= ~(1 << BTN_HOME);
  PORTB |= (1 << BTN_HOME);

  usart_print_string("=== TEST 5: BUTTONS ===\n");
  usart_print_string("Press in order: Joystick SW, BACK Button (PD3), HOME "
                     "Button (PB0)\n\n");

  while (1) {
    uint8_t sw = !(PIND & (1 << JOY_SW_PIN));
    uint8_t back = !(PIND & (1 << BTN_BACK));
    uint8_t home = !(PINB & (1 << BTN_HOME));

    if (sw)
      usart_print_string("[JOY SW] ");
    if (back)
      usart_print_string("[BACK] ");
    if (home)
      usart_print_string("[HOME] ");

    if (sw || back || home)
      usart_print_string("\n");

    _delay_ms(100);
  }
}

// ==================== MAIN ====================
int main(void) {
#if TEST_TASK == 1
  test_led();
#elif TEST_TASK == 2
  test_joystick();
#elif TEST_TASK == 3
  test_buzzer();
#elif TEST_TASK == 4
  test_oled();
#elif TEST_TASK == 5
  test_buttons();
#elif TEST_TASK == 6
  // ============ MINI GAME CONSOLE ============
  i2c_init();
  ssd1306_init();
  adc_init();
  buzzer_init();
  pwm_init();

  // Configure button pins as INPUT with internal Pull-Up enabled
  DDRD &= ~((1 << JOY_SW_PIN) | (1 << BTN_BACK));
  PORTD |= (1 << JOY_SW_PIN) | (1 << BTN_BACK);

  DDRB &= ~(1 << BTN_HOME);
  PORTB |= (1 << BTN_HOME);

  // LED starts turned off
  set_rgb_color(0, 0, 0);

  // Breakout settings variables
  uint8_t ball_spd = 30;
  uint8_t paddle_spd = 30;
  uint8_t start_lives = 3;

  // Player name variables
  char player_name[MAX_NAME_LEN + 1] = {0};
  uint8_t name_len = 0;

  // Navigation and selection variables
  uint8_t prev_btn = 0;
  uint8_t prev_back = 0;
  uint8_t prev_home = 0;
  uint8_t current_game = GAME_BREAKOUT;

  enum { ST_CON, ST_GM, ST_LB, ST_MD, ST_NM, ST_BS, ST_BK, ST_FP };
  uint8_t state = ST_CON;
  uint8_t menu_sel = 0;
  uint8_t console_sel = 0;
  uint8_t mode_sel = 0;
  uint8_t prev_jy = 1;
  uint8_t prev_jx = 1;
  uint8_t grid_r = 0;
  uint8_t grid_c = 0;
  uint8_t name_hold = 0;
  uint8_t set_cursor = 0;
  uint8_t prev_jy_p = 1;
  uint8_t lb_return = ST_GM;
  uint8_t lb_scroll = 0;

  // Game structures
  BreakoutState bk;
  FlappyState fp;

  while (1) {
    uint16_t joy_x = adc_read(JOY_X_CHANNEL), joy_y = adc_read(JOY_Y_CHANNEL);
    uint8_t btn = !(PIND & (1 << JOY_SW_PIN)),
            back = !(PIND & (1 << BTN_BACK)),
            home_btn = !(PINB & (1 << BTN_HOME));
    uint8_t bp = btn && !prev_btn,
            bkp = back && !prev_back,
            home_p = home_btn && !prev_home;

    if (home_p) {
      buzzer_stop();
      set_rgb_color(0, 0, 0);
      state = ST_CON;
    }

    uint8_t jy = (joy_y < 300)   ? 0
                 : (joy_y > 724) ? 2
                                 : 1,
            jx = (joy_x < 400)   ? 0
                 : (joy_x > 624) ? 2
                                 : 1;

    switch (state) {
    // ---- CONSOLE: select game ----
    case ST_CON:
      if (jy != prev_jy) {
        if (jy == 0 && console_sel > 0)
          console_sel--;
        if (jy == 2 && console_sel < 1)
          console_sel++;
      }
      if (bp) {
        current_game = console_sel;
        menu_sel = 0;
        state = ST_GM;
      }
      ssd1306_clear();
      ssd1306_draw_string(16, 2, "GAME CONSOLE");
      ssd1306_draw_string(34, 24, "BREAKOUT");
      ssd1306_draw_string(34, 36, "FLAPPY BIRD");
      ssd1306_draw_char(26, 24 + console_sel * 12, '>');
      ssd1306_update();
      break;

    // ---- GAME MENU: play / leaderboard ----
    case ST_GM:
      if (bkp) {
        console_sel = current_game;
        state = ST_CON;
        break;
      }
      if (jy != prev_jy) {
        if (jy == 0 && menu_sel > 0)
          menu_sel--;
        if (jy == 2 && menu_sel < 1)
          menu_sel++;
      }
      if (bp) {
        if (menu_sel == 0) {
          mode_sel = 0;
          state = ST_MD;
        } else {
          lb_return = ST_GM;
          lb_scroll = 0;
          state = ST_LB;
        }
      }
      ssd1306_clear();
      ssd1306_draw_string(current_game == 0 ? 34 : 22, 6,
                          current_game == 0 ? "BREAKOUT" : "FLAPPY BIRD");
      ssd1306_draw_string(34, 26, "PLAY GAME");
      ssd1306_draw_string(34, 38, "LEADERBOARD");
      ssd1306_draw_char(26, menu_sel == 0 ? 26 : 38, '>');
      ssd1306_update();
      break;

    // ---- MODE SELECT: ranked / for fun ----
    case ST_MD:
      if (bkp) {
        menu_sel = 0;
        state = ST_GM;
        break;
      }
      if (jy != prev_jy) {
        if (jy == 0 && mode_sel > 0)
          mode_sel--;
        if (jy == 2 && mode_sel < 1)
          mode_sel++;
      }
      if (bp) {
        if (mode_sel == 0) {
          // RANKED: go to name entry
          name_len = 0;
          player_name[0] = '\0';
          grid_r = 0;
          grid_c = 0;
          name_hold = 0;
          state = ST_NM;
        } else {
          // FOR FUN: skip name, go directly to game
          player_name[0] = '\0';
          name_len = 0;
          if (current_game == GAME_BREAKOUT) {
            set_cursor = 0;
            state = ST_BS;
          } else {
            for (uint8_t i = 0; i <= MAX_NAME_LEN; i++) {
              fp.player_name[i] = '\0';
            }
            flappy_init(&fp);
            prev_jy_p = 1;
            state = ST_FP;
          }
        }
      }
      ssd1306_clear();
      ssd1306_draw_string(28, 6, "SELECT MODE");
      ssd1306_draw_string(34, 26, "RANKED");
      ssd1306_draw_string(34, 38, "FOR FUN");
      ssd1306_draw_char(26, 26 + mode_sel * 12, '>');
      ssd1306_update();
      break;

    // ---- LEADERBOARD ----
    case ST_LB: {
      if (bp || bkp) {
        state = lb_return;
      }
      ScoreEntry entries[MAX_SCORES];
      uint8_t cnt = scores_load(current_game, entries);

      if (jy != prev_jy) {
        if (jy == 2 && lb_scroll + 5 < cnt)
          lb_scroll++;
        if (jy == 0 && lb_scroll > 0)
          lb_scroll--;
      }

      ssd1306_clear();
      ssd1306_draw_string(22, 0, "LEADERBOARD");

      for (uint8_t i = 0; i < 5; i++) {
        uint8_t idx = lb_scroll + i;
        if (idx >= cnt)
          break;

        uint8_t y = 12 + i * 10;
        ssd1306_draw_number(0, y, idx + 1);
        ssd1306_draw_char((idx + 1 < 10) ? 6 : 12, y, '.');
        ssd1306_draw_string((idx + 1 < 10) ? 14 : 20, y, entries[idx].name);
        ssd1306_draw_number(90, y, entries[idx].score);
      }

      if (cnt == 0) {
        ssd1306_draw_string(22, 28, "NO SCORES YET");
      } else {
        if (lb_scroll > 0)
          ssd1306_draw_char(120, 12, '^');
        if (lb_scroll + 5 < cnt)
          ssd1306_draw_char(120, 42, 'v');
      }

      ssd1306_draw_string(28, 57, "PRESS BACK");
      ssd1306_update();
      break;
    }

    // ---- NAME ENTRY ----
    case ST_NM: {
      if (bkp) {
        menu_sel = 0;
        state = ST_GM;
        break;
      }

      uint8_t wm = 0;
      if (jx != 1 || jy != 1) {
        name_hold++;
        if (name_hold == 1 || (name_hold > 10 && name_hold % 3 == 0)) {
          wm = 1;
        }
      } else {
        name_hold = 0;
      }

      if (wm) {
        if (jy == 0 && grid_r > 0)
          grid_r--;
        if (jy == 2 && grid_r < 3)
          grid_r++;
        if (jx == 0 && grid_c > 0)
          grid_c--;
        if (jx == 2 && grid_c < 6)
          grid_c++;
      }

      uint8_t idx = grid_r * 7 + grid_c;
      if (idx > 27) {
        grid_c = 6;
        idx = 27;
      }

      if (bp) {
        if (idx < 26) {
          if (name_len < MAX_NAME_LEN) {
            player_name[name_len++] = 'A' + idx;
            player_name[name_len] = '\0';
          }
        } else if (idx == 26) {
          if (name_len > 0) {
            name_len--;
            player_name[name_len] = '\0';
          }
        } else if (name_len > 0) {
          if (current_game == GAME_BREAKOUT) {
            set_cursor = 0;
            state = ST_BS;
          } else {
            for (uint8_t i = 0; i <= MAX_NAME_LEN; i++) {
              fp.player_name[i] = player_name[i];
            }
            flappy_init(&fp);
            prev_jy_p = 1;
            state = ST_FP;
          }
        }
      }

      ssd1306_clear();
      ssd1306_draw_string(28, 0, "ENTER NAME");
      if (name_len > 0) {
        ssd1306_draw_string(4, 12, player_name);
      }
      if (name_len < MAX_NAME_LEN) {
        ssd1306_draw_hline(4 + name_len * 6, 19, 5, 1);
      }

      for (uint8_t r = 0; r < 4; r++) {
        for (uint8_t c = 0; c < 7; c++) {
          uint8_t i = r * 7 + c;
          if (i > 27)
            break;

          uint8_t gx = 4 + c * 18;
          uint8_t gy = 26 + r * 10;
          char ch = i < 26 ? 'A' + i : i == 26 ? '<' : '>';
          ssd1306_draw_char(gx + 2, gy + 1, ch);

          if (r == grid_r && c == grid_c) {
            ssd1306_draw_hline(gx, gy, 12, 1);
            ssd1306_draw_hline(gx, gy + 9, 12, 1);
            ssd1306_draw_vline(gx, gy, 10, 1);
            ssd1306_draw_vline(gx + 11, gy, 10, 1);
          }
        }
      }
      ssd1306_update();
      break;
    }

    // ---- BREAKOUT SETTINGS ----
    case ST_BS: {
      if (bkp) {
        grid_r = 0;
        grid_c = 0;
        name_hold = 0;
        state = ST_NM;
        break;
      }
      if (jy != prev_jy) {
        if (jy == 0 && set_cursor > 0)
          set_cursor--;
        if (jy == 2 && set_cursor < 2)
          set_cursor++;
      }

      if (set_cursor == 2) {
        if (jx != prev_jx) {
          if (jx == 0 && start_lives > 1)
            start_lives--;
          if (jx == 2 && start_lives < 3)
            start_lives++;
        }
      } else {
        uint8_t *v = (set_cursor == 0) ? &ball_spd : &paddle_spd;
        if (joy_x < 200 && *v > 11)
          (*v) -= 2;
        else if (joy_x < 400 && *v > 10)
          (*v)--;
        else if (joy_x > 824 && *v < 49)
          (*v) += 2;
        else if (joy_x > 624 && *v < 50)
          (*v)++;
      }

      if (bp) {
        bk.ball_speed = ball_spd;
        bk.paddle_speed = paddle_spd;
        bk.start_lives = start_lives;
        for (uint8_t i = 0; i <= MAX_NAME_LEN; i++) {
          bk.player_name[i] = player_name[i];
        }
        breakout_init(&bk);
        prev_jy_p = 1;
        state = ST_BK;
      }

      ssd1306_clear();
      ssd1306_draw_string(34, 0, "SETTINGS");

      ssd1306_draw_string(10, 14, "BALL");
      ssd1306_draw_hline(68, 13, 42, 1);
      ssd1306_draw_hline(68, 22, 42, 1);
      ssd1306_draw_vline(68, 13, 10, 1);
      ssd1306_draw_vline(109, 13, 10, 1);
      if (ball_spd > 10) {
        ssd1306_fill_rect(69, 14, ball_spd - 10, 8, 1);
      }

      ssd1306_draw_string(10, 28, "PADDLE");
      ssd1306_draw_hline(68, 27, 42, 1);
      ssd1306_draw_hline(68, 36, 42, 1);
      ssd1306_draw_vline(68, 27, 10, 1);
      ssd1306_draw_vline(109, 27, 10, 1);
      if (paddle_spd > 10) {
        ssd1306_fill_rect(69, 28, paddle_spd - 10, 8, 1);
      }

      ssd1306_draw_string(10, 42, "LIVES");
      for (uint8_t i = 0; i < 3; i++) {
        uint8_t hx = 68 + i * 14;
        if (i < start_lives) {
          ssd1306_fill_rect(hx, 42, 10, 8, 1);
        } else {
          ssd1306_draw_hline(hx, 42, 10, 1);
          ssd1306_draw_hline(hx, 49, 10, 1);
          ssd1306_draw_vline(hx, 42, 8, 1);
          ssd1306_draw_vline(hx + 9, 42, 8, 1);
        }
      }

      if (set_cursor == 0)
        ssd1306_draw_char(2, 14, '>');
      else if (set_cursor == 1)
        ssd1306_draw_char(2, 28, '>');
      else
        ssd1306_draw_char(2, 42, '>');

      ssd1306_draw_string(16, 56, "PRESS TO START");
      ssd1306_update();
      break;
    }

    // ---- BREAKOUT GAMEPLAY ----
    case ST_BK:
      if (bk.game_over || bk.game_won) {
        if (jy != prev_jy_p) {
          if (jy == 0 && bk.end_selection > 0)
            bk.end_selection--;
          if (jy == 2 && bk.end_selection < 3)
            bk.end_selection++;
        }
        if (jy != prev_jy_p)
          prev_jy_p = jy;

        if (bp) {
          if (bk.end_selection == 0) {
            bk.ball_speed = ball_spd;
            bk.paddle_speed = paddle_spd;
            bk.start_lives = start_lives;
            for (uint8_t i = 0; i <= MAX_NAME_LEN; i++) {
              bk.player_name[i] = player_name[i];
            }
            breakout_init(&bk);
          } else if (bk.end_selection == 1) {
            set_cursor = 0;
            state = ST_BS;
          } else if (bk.end_selection == 2) {
            set_rgb_color(0, 0, 0);
            menu_sel = 0;
            state = ST_GM;
          } else {
            lb_return = ST_BK;
            lb_scroll = 0;
            state = ST_LB;
          }
        }
      } else if (bk.paused) {
        if (jy != prev_jy_p) {
          if (jy == 0 && bk.pause_selection > 0)
            bk.pause_selection--;
          if (jy == 2 && bk.pause_selection < 3)
            bk.pause_selection++;
        }
        if (jy != prev_jy_p)
          prev_jy_p = jy;

        if (bp) {
          if (bk.pause_selection == 0) {
            bk.paused = 0;
          } else if (bk.pause_selection == 1) {
            bk.ball_speed = ball_spd;
            bk.paddle_speed = paddle_spd;
            bk.start_lives = start_lives;
            for (uint8_t i = 0; i <= MAX_NAME_LEN; i++) {
              bk.player_name[i] = player_name[i];
            }
            breakout_init(&bk);
          } else if (bk.pause_selection == 2) {
            set_cursor = 0;
            state = ST_BS;
          } else {
            set_rgb_color(0, 0, 0);
            menu_sel = 0;
            state = ST_GM;
          }
        }
      } else {
        if (bp) {
          if (!bk.ball_launched) {
            breakout_launch_ball(&bk);
          } else {
            bk.paused = 1;
            bk.pause_selection = 0;
          }
        }
        breakout_update(&bk, joy_x);
      }
      breakout_render(&bk);
      break;

    // ---- FLAPPY BIRD GAMEPLAY ----
    case ST_FP:
      if (fp.game_over) {
        if (jy != prev_jy_p) {
          if (jy == 0 && fp.end_selection > 0)
            fp.end_selection--;
          if (jy == 2 && fp.end_selection < 3)
            fp.end_selection++;
        }
        if (jy != prev_jy_p)
          prev_jy_p = jy;

        if (bp) {
          if (fp.end_selection == 0) {
            for (uint8_t i = 0; i <= MAX_NAME_LEN; i++) {
              fp.player_name[i] = player_name[i];
            }
            flappy_init(&fp);
          } else if (fp.end_selection == 1) {
            set_rgb_color(0, 0, 0);
            menu_sel = 0;
            state = ST_GM;
          } else if (fp.end_selection == 2) {
            set_rgb_color(0, 0, 0);
            console_sel = current_game;
            state = ST_CON;
          } else {
            lb_return = ST_FP;
            lb_scroll = 0;
            state = ST_LB;
          }
        }
      } else if (fp.paused) {
        if (jy != prev_jy_p) {
          if (jy == 0 && fp.pause_selection > 0)
            fp.pause_selection--;
          if (jy == 2 && fp.pause_selection < 3)
            fp.pause_selection++;
        }
        if (jy != prev_jy_p)
          prev_jy_p = jy;

        if (bp) {
          if (fp.pause_selection == 0) {
            fp.paused = 0;
          } else if (fp.pause_selection == 1) {
            for (uint8_t i = 0; i <= MAX_NAME_LEN; i++) {
              fp.player_name[i] = player_name[i];
            }
            flappy_init(&fp);
          } else if (fp.pause_selection == 2) {
            set_rgb_color(0, 0, 0);
            menu_sel = 0;
            state = ST_GM;
          } else {
            set_rgb_color(0, 0, 0);
            console_sel = current_game;
            state = ST_CON;
          }
        }
      } else {
        if (bp) {
          flappy_flap(&fp);
        } else if (btn && fp.started && !fp.paused && !fp.game_over) {
          fp.bird_vy = FLAPPY_FLAP;
        }
        flappy_update(&fp);
      }
      flappy_render(&fp);
      break;
    } // switch
    prev_btn = btn;
    prev_back = back;
    prev_home = home_btn;
    prev_jy = jy;
    prev_jx = jx;
    if (state == ST_BS)
      _delay_ms(50);
    else
      _delay_ms(33);
  } // while
#else
  DDRB |= (1 << PB5);
  while (1) {
    PORTB ^= (1 << PB5);
    _delay_ms(500);
  }
#endif
  return 0;
}
