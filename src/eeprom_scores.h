#ifndef EEPROM_SCORES_H
#define EEPROM_SCORES_H

#include <stdint.h>

#define MAX_NAME_LEN 8
#define MAX_SCORES   25

// Game IDs for separate leaderboards
#define GAME_BREAKOUT 0
#define GAME_FLAPPY   1

typedef struct {
    char     name[MAX_NAME_LEN + 1]; // 8 chars + null
    uint16_t score;
} ScoreEntry;

// Loads scores from EEPROM for a game. Returns the number of entries.
uint8_t scores_load(uint8_t game_id, ScoreEntry entries[]);

// Saves a new score (sorted insertion, deduplication by name).
void scores_save(uint8_t game_id, const char *name, uint16_t score);

#endif // EEPROM_SCORES_H
