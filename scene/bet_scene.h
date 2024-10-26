#pragma once
#include <input/input.h>
#include "../util/scene.h"


void bet_start(void *data, SceneData* sceneData);
void bet_render(void *data, SceneData* sceneData);
void bet_render_ui(void *data, SceneData* sceneData);
void bet_input(void *data, SceneData* sceneData, InputKey key, InputType type);