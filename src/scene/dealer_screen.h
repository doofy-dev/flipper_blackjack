#pragma once

#include "../util/scene.h"

void dealer_start(void *data, SceneData* sceneData);
void dealer_render(void *data, SceneData* sceneData);
void dealer_render_ui(void *data, SceneData* sceneData);
void dealer_update(void *data, SceneData* sceneData);
void dealer_input(void *data, SceneData* sceneData, InputKey key, InputType type);