#include "eeprom_scores.h"
#include <avr/eeprom.h>
#include <string.h>

// EEPROM Layout per joc:
// Byte 0:       count (0-25)
// Bytes 1-275:  25 entries x 11 bytes (9 name bytes + 2 score bytes)
// Total per joc: 276 bytes
// Game 0 (Breakout): offset 0
// Game 1 (Flappy):   offset 276

#define ENTRY_SIZE (MAX_NAME_LEN + 1 + 2)  // 11 bytes
#define GAME_BLOCK_SIZE (1 + MAX_SCORES * ENTRY_SIZE) // 276 bytes

static uint16_t game_base(uint8_t game_id) {
    return (uint16_t)game_id * GAME_BLOCK_SIZE;
}

static void read_entry(uint8_t game_id, uint8_t index, ScoreEntry *e) {
    uint16_t addr = game_base(game_id) + 1 + (uint16_t)index * ENTRY_SIZE;
    for (uint8_t i = 0; i <= MAX_NAME_LEN; i++) {
        e->name[i] = eeprom_read_byte((uint8_t *)(addr + i));
    }
    e->score = eeprom_read_word((uint16_t *)(addr + MAX_NAME_LEN + 1));
}

static void write_entry(uint8_t game_id, uint8_t index, const ScoreEntry *e) {
    uint16_t addr = game_base(game_id) + 1 + (uint16_t)index * ENTRY_SIZE;
    for (uint8_t i = 0; i <= MAX_NAME_LEN; i++) {
        eeprom_update_byte((uint8_t *)(addr + i), e->name[i]);
    }
    eeprom_update_word((uint16_t *)(addr + MAX_NAME_LEN + 1), e->score);
}

uint8_t scores_load(uint8_t game_id, ScoreEntry entries[]) {
    uint8_t count = eeprom_read_byte((uint8_t *)game_base(game_id));
    if (count > MAX_SCORES) count = 0; // Uninitialized EEPROM (0xFF)
    for (uint8_t i = 0; i < count; i++) {
        read_entry(game_id, i, &entries[i]);
    }
    return count;
}

void scores_save(uint8_t game_id, const char *name, uint16_t score) {
    ScoreEntry entries[MAX_SCORES];
    uint8_t count = scores_load(game_id, entries);
    
    // Check if the name already exists
    for (uint8_t i = 0; i < count; i++) {
        if (strncmp(entries[i].name, name, MAX_NAME_LEN) == 0) {
            if (score <= entries[i].score) return; // Old score is better or equal
            // Delete the old entry
            for (uint8_t j = i; j < count - 1; j++) {
                entries[j] = entries[j + 1];
            }
            count--;
            break;
        }
    }
    
    // Find insertion position (descending sort)
    uint8_t pos = count;
    for (uint8_t i = 0; i < count; i++) {
        if (score > entries[i].score) { pos = i; break; }
    }
    if (pos >= MAX_SCORES) return;
    
    uint8_t new_count = count < MAX_SCORES ? count + 1 : MAX_SCORES;
    for (uint8_t i = new_count - 1; i > pos; i--) {
        entries[i] = entries[i - 1];
    }
    
    strncpy(entries[pos].name, name, MAX_NAME_LEN);
    entries[pos].name[MAX_NAME_LEN] = '\0';
    entries[pos].score = score;
    
    // Write everything to EEPROM
    eeprom_update_byte((uint8_t *)game_base(game_id), new_count);
    for (uint8_t i = 0; i < new_count; i++) {
        write_entry(game_id, i, &entries[i]);
    }
}
