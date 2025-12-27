#pragma once
#include <furi.h>
#include "../util/scene.h"

void split_start(void *data, SceneData* sceneData);
void split_render(void *data, SceneData* sceneData);
//void split_render_ui(void *data, SceneData* sceneData);
void split_update(void *data, SceneData* sceneData);
void split_input(void *data, SceneData* sceneData, InputKey key, InputType type);