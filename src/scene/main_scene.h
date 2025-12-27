#pragma once

#include <furi.h>
#include "../util/scene.h"

void main_start(void *data, SceneData* sceneData);
void main_render(void *data, SceneData* sceneData);
void main_render_ui(void *data, SceneData* sceneData);
void main_update(void *data, SceneData* sceneData);
void main_input(void *data, SceneData* sceneData, InputKey key, InputType type);