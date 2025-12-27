#include "settings.h"
#include <storage/storage.h>
#include <furi.h>

#define SAVE_DIRECTORY EXT_PATH("apps_data/blackjack")
#define SAVE_FILENAME SAVE_DIRECTORY "/blackjack.save"

typedef struct {
    bool sound_enabled;
    uint8_t bet;
    uint32_t balance;
    uint8_t balance_mul;
} SaveData;

void save_settings(GameState *state) {
    Storage *storage = furi_record_open(RECORD_STORAGE);

    if (storage_common_stat(storage, SAVE_DIRECTORY, NULL) == FSE_NOT_EXIST) {
        if (!storage_simply_mkdir(storage, SAVE_DIRECTORY)) {
            return;
        }
    }

    File *file = storage_file_alloc(storage);
    SaveData data = {
        .balance_mul = state->state->balance_mul,
        .balance = state->state->balance,
        .bet = state->state->bet,
        .sound_enabled=state->sound_enabled
    };
    if (storage_file_open(file, SAVE_FILENAME, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(file, &data, sizeof(SaveData));
    }

    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);
}

void load_settings(GameState *state) {
    Storage *storage = furi_record_open(RECORD_STORAGE);
    File *file = storage_file_alloc(storage);
    uint16_t bytes_readed = 0;
    SaveData data;

    if (storage_file_open(file, SAVE_FILENAME, FSAM_READ, FSOM_OPEN_EXISTING)) {
        bytes_readed = storage_file_read(file, &data, sizeof(SaveData));
        if (bytes_readed == sizeof(SaveData)) {
            state->sound_enabled = data.sound_enabled;
            state->state->bet = data.bet;
            state->state->balance_mul = data.balance_mul;
            state->state->balance = data.balance;
        }
    }

    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);
}
