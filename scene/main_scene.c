#include "main_scene.h"
#include "../assets.h"
#include "../game_state.h"
#include "../settings.h"
#include "../util/helpers.h"
#include "../notes.h"

static Vector logo_pos = (Vector) {62, 32};
static Vector cards_pos = (Vector) {111, 30};
static Vector speaker_on_pos = (Vector) {18, 57};
static Vector speaker_off_pos = (Vector) {14, 57};
static Vector left_arrow_pos = (Vector) {3, 57};
static Vector play_pos = (Vector) {SCREEN_WIDTH - 4, 58};

static size_t last_start = 0;
static int8_t note = 0;

void main_start(void *data, SceneData *sceneData) {
    UNUSED(data);
    UNUSED(sceneData);
    //load settings
}

void main_render(void *data, SceneData *sceneData) {
    GameState *state = (GameState *) data;
    buffer_set_sprite_rotation(0);

    buffer_draw_all(sceneData->buffer, (Buffer *) &sprite_main_image, &cards_pos);

    buffer_draw_all(sceneData->buffer, (Buffer *) &sprite_logo, &logo_pos);

    if (state->sound_enabled)
        buffer_draw_all(sceneData->buffer, (Buffer *) &sprite_speaker_on, &speaker_on_pos);
    else
        buffer_draw_all(sceneData->buffer, (Buffer *) &sprite_speaker_off, &speaker_off_pos);

    buffer_set_sprite_rotation(180);
    buffer_draw_all(sceneData->buffer, (Buffer *) &sprite_play, &left_arrow_pos);

    buffer_set_sprite_rotation(0);
    buffer_draw_all(sceneData->buffer, (Buffer *) &sprite_ok, &play_pos);
}

void main_render_ui(void *data, SceneData *sceneData) {
    UNUSED(data);
    UNUSED(sceneData);
    canvas_set_color(sceneData->canvas, ColorBlack);
    canvas_set_font(sceneData->canvas, FontSecondary);
    canvas_draw_str(sceneData->canvas, 100, 61, "Play");
}

void main_update(void *data, SceneData *sceneData) {
    sceneData->clear_buffer = sceneData->dirty;
    GameState *state = (GameState *) data;
    if (state->sound_enabled) {
        size_t t = curr_time();
        if ((last_start - t) > 250) {
            if (note >= 0) {
                music[0] = music_notes[note];

                notification_message_block(state->notification_app, (const NotificationSequence *) &music);

                last_start = t;
            }
            note = (note + 1) % 32;
        }
    }

}

void main_input(void *data, SceneData *sceneData, InputKey key, InputType type) {
    GameState *state = (GameState *) data;

    if (type == InputTypePress) {
        if (key == InputKeyLeft) {
            state->sound_enabled = !state->sound_enabled;
            sceneData->dirty = true;
            note=0;
            save_settings(state);
        } else if (key == InputKeyOk) {
            sceneData->scene_switch = Next;
        }
    }

}