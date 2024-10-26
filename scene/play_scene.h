#pragma once

#include <input/input.h>
#include "../util/scene.h"


void play_start(void *data, SceneData* sceneData);
void play_render(void *data, SceneData* sceneData);
void play_render_ui(void *data, SceneData* sceneData);
void play_update(void *data, SceneData* sceneData);
void play_input(void *data, SceneData* sceneData, InputKey key, InputType type);