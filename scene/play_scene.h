#pragma once

#include <input/input.h>
#include "../util/scene.h"
#include "../game_state.h"


void play_start(void *data, SceneData* sceneData);
void play_render(void *data, SceneData* sceneData);
void play_render_ui(void *data, SceneData* sceneData);
void play_update(void *data, SceneData* sceneData);
void play_input(void *data, SceneData* sceneData, InputKey key, InputType type);

bool can_split(GameState *game_state, List *hand);
bool can_double(GameState *game_state);
void check_menu(GameState *game_state, bool toNext);