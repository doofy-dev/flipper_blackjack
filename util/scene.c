#include <notification/notification_messages.h>
#include "scene.h"
#include "helpers.h"
#include "audio.h"

static size_t curr_frame_time = 0;
static size_t last_frame_time = 0;
static SceneData *sceneData = NULL;

void start_scene(void *data);

void handle_scene_switch(void *data);


void prepare_scenes(Canvas *canvas) {
    sceneData = allocate(sizeof(SceneData));
    sceneData->dirty = true;
    sceneData->scenes = list_make();
    sceneData->buffer = buffer_create(SCREEN_WIDTH, SCREEN_HEIGHT, false);
    sceneData->canvas = canvas;

    sceneData->notification_app = (NotificationApp *) furi_record_open(RECORD_NOTIFICATION);
    notification_message_block(sceneData->notification_app, &sequence_display_backlight_enforce_on);
}

void free_scenes() {
    sceneData->canvas = NULL;
    list_clear(sceneData->scenes);
    buffer_release(sceneData->buffer);

    notification_message_block(sceneData->notification_app, &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);

    release(sceneData->scenes);
    release(sceneData);
}

void add_scene(Scene *scene) {
    FURI_LOG_D("SCENE", "ADDING scene %s", scene->name);
    list_push_back(scene, sceneData->scenes);
}

void set_scene(void *data, uint8_t index) {
    sceneData->scene_switch = Index;
    sceneData->scene_index = index;
    handle_scene_switch(data);
}

void next_scene(void *data) {
    sceneData->current_scene = sceneData->current_scene->next;
    if (sceneData->current_scene == NULL)
        sceneData->current_scene = sceneData->scenes->head;

    start_scene(data);
}

void prev_scene(void *data) {
    sceneData->current_scene = sceneData->current_scene->prev;
    if (sceneData->current_scene == NULL) {
        sceneData->current_scene = sceneData->scenes->head;
    }

    start_scene(data);
}
void start_scene(void *data) {
    if (sceneData->scenes->head == NULL) {
        FURI_LOG_W("SCENE", "Can't start scene, list empty");
        return;
    }

    if (sceneData->current_scene) {
        Scene *s = (Scene *) sceneData->current_scene->data;
        FURI_LOG_D("SCENE", "Starting scene %s", s->name);
        if (s->start)
            s->start(data, sceneData);
        else
            FURI_LOG_W("SCENE", "no start function");
        sceneData->dirty = true;
    } else {
        FURI_LOG_W("SCENE", "No scene data");
    }
}

void handle_scene_switch(void *data) {
    if (sceneData->scene_switch == Next) {
        next_scene(data);
    } else if (sceneData->scene_switch == Prev) {
        prev_scene(data);
    } else if (sceneData->scene_switch == Start) {
        start_scene(data);
    } else if (sceneData->scene_switch == Index) {

        ListItem *curr = sceneData->scenes->head;

        uint8_t i = 0;
        check_pointer(curr);
        while (curr) {
            if (i == sceneData->scene_index) {
                sceneData->current_scene = curr;
                start_scene(data);
                break;
            }
            i++;
            curr = curr->next;
        }
        check_pointer(sceneData->current_scene);
    }

    sceneData->scene_switch = Stay;
}

void update_scene(void *data) {
    if (!sceneData->current_scene || !sceneData->current_scene->data) return;

    Scene *s = (Scene *) sceneData->current_scene->data;

    if (!s) return;

    if (last_frame_time == 0) {
        last_frame_time = curr_time();
    }

    curr_frame_time = curr_time();
    sceneData->delta_time = (float)(curr_frame_time - last_frame_time) / 64000000.0f;
    last_frame_time = curr_frame_time;

    if (s->update)
    {
        s->update(data, sceneData);
    }

    update_audio(sceneData->notification_app);

    if (sceneData->scene_switch != Stay) {
        handle_scene_switch(data);
    }
}

void render_scene(void *data) {
    if (!sceneData->current_scene || !sceneData->current_scene->data) return;

    Scene *s = (Scene *) sceneData->current_scene->data;

    if (!sceneData->dirty) return;
    if (sceneData->canvas && sceneData->buffer) {
//        FURI_LOG_W("STATE", "canvas_reset");
        canvas_reset(sceneData->canvas);
//        FURI_LOG_W("STATE", "buffer_swap_back");
        buffer_swap_back(sceneData->buffer);
//        FURI_LOG_W("STATE", "RENDER");

        if (s->render) {
            s->render(data, sceneData);
            sceneData->dirty = false;
        }
        buffer_render(sceneData->buffer, sceneData->canvas);

//        FURI_LOG_W("STATE", "RENDER UI");
        if (s->render_ui) {
            s->render_ui(data, sceneData);
        }

//        FURI_LOG_W("STATE", "CLEAR");
        if (sceneData->clear_buffer) {
            buffer_clear(sceneData->buffer);
        }
//        FURI_LOG_W("STATE", "canvas_commit");
        canvas_commit(sceneData->canvas);

        sceneData->clear_buffer = true;
    }
}

void input_scene(void *data, InputKey key, InputType type) {
    if (!sceneData || !sceneData->current_scene || !sceneData->current_scene->data) return;

    Scene *s = (Scene *) sceneData->current_scene->data;
    if (s && s->input) {
        s->input(data, sceneData, key, type);
    }
}