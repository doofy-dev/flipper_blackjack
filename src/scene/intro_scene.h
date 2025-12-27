#pragma once
#include <input/input.h>
#include "../util/scene.h"


void intro_start(void *data, SceneData* sceneData);
void intro_render(void *data, SceneData* sceneData);
void intro_render_ui(void *data, SceneData* sceneData);
void intro_update(void *data, SceneData* sceneData);
void intro_input(void *data, SceneData* sceneData, InputKey key, InputType type);