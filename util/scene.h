#pragma once
#include <furi.h>
#include <input/input.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include "buffer.h"
#include "list.h"

struct Scene;
typedef struct Scene Scene;

typedef enum{
    Stay,
    Next,
    Prev,
    Index,
    Start
} SceneSwitch;

typedef struct {
    bool dirty;
    bool clear_buffer;
    float delta_time;
    List *scenes;
    ListItem *current_scene;
    NotificationApp *notification_app;
    SceneSwitch scene_switch;
    uint8_t scene_index;
    Buffer *buffer;
    Canvas *canvas;
} SceneData;

struct Scene{
    const char* name;
    void (*start)(void *data, SceneData* sceneData);
    void (*end)(void *data, SceneData* sceneData);
    void (*render)(void *data, SceneData* sceneData);
    void (*render_ui)(void *data, SceneData* sceneData);

    void (*update)(void *data, SceneData* sceneData);

    void (*input)(void *data, SceneData* sceneData, InputKey key, InputType type);
};

void prepare_scenes(Canvas *canvas);
void free_scenes();

void add_scene(Scene* scene);
void set_scene(void* data, uint8_t index);
void update_scene(void* data);
void render_scene(void* data);
void input_scene(void* data, InputKey key, InputType type);
