#pragma once
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>

#include "game_state.h"

void save_settings(GameState *state);
void load_settings(GameState *state);
