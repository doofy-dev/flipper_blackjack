#include <notification/notification_messages.h>
#include "main_scene.h"
#include "../game_state.h"
#include "../settings.h"
#include "../util/helpers.h"
#include "blackjack_icons.h"
#include "../util/asset.h"
#include "../util/audio.h"
#include "../sounds.h"

static Vector logo_pos = (Vector) {62, 32};
static Vector cards_pos = (Vector) {111, 30};
static Vector speaker_on_pos = (Vector) {18, 57};
static Vector speaker_off_pos = (Vector) {14, 57};
static Vector left_arrow_pos = (Vector) {3, 57};
static Vector play_pos = (Vector) {SCREEN_WIDTH - 4, 58};

static Buffer *icon_main, *icon_logo, *icon_speaker_on, *icon_speaker_off, *icon_play, *icon_ok;

void main_start(void *data, SceneData *sceneData) {
    UNUSED(sceneData);
    GameState *state = (GameState *) data;

    icon_main = asset_get_icon(&I_main_image);
    icon_logo = asset_get_icon(&I_logo);
    icon_speaker_on = asset_get_icon(&I_speaker_on);
    icon_speaker_off = asset_get_icon(&I_speaker_off);
    icon_play = asset_get_icon(&I_play);
    icon_ok = asset_get_icon(&I_ok);

    set_volume(0.25f);
    set_audio(&menu_music);
    if (state->sound_enabled)
        play_audio();
}

void main_render(void *data, SceneData *sceneData) {
    GameState *state = (GameState *) data;
    buffer_set_sprite_rotation(0);

    buffer_draw_all(sceneData->buffer, icon_main, &cards_pos);

    buffer_draw_all(sceneData->buffer, icon_logo, &logo_pos);

    if (state->sound_enabled)
        buffer_draw_all(sceneData->buffer, icon_speaker_on, &speaker_on_pos);
    else
        buffer_draw_all(sceneData->buffer, icon_speaker_off, &speaker_off_pos);

    buffer_set_sprite_rotation(180);
    buffer_draw_all(sceneData->buffer, icon_play, &left_arrow_pos);

    buffer_set_sprite_rotation(0);
    buffer_draw_all(sceneData->buffer, icon_ok, &play_pos);
}

void main_render_ui(void *data, SceneData *sceneData) {
    UNUSED(data);
    UNUSED(sceneData);
    canvas_set_color(sceneData->canvas, ColorBlack);
    canvas_set_font(sceneData->canvas, FontSecondary);
    canvas_draw_str(sceneData->canvas, 100, 61, "Play");
}

void main_update(void *data, SceneData *sceneData) {
    UNUSED(data);
    UNUSED(sceneData);
}

void main_input(void *data, SceneData *sceneData, InputKey key, InputType type) {
    GameState *state = (GameState *) data;

    if (type == InputTypePress) {
        if (key == InputKeyLeft) {
            state->sound_enabled = !state->sound_enabled;
            if (state->sound_enabled) {
                play_audio();
            } else {
                stop_audio();
            }
            save_settings(state);
        } else if (key == InputKeyOk) {
            sceneData->scene_switch = Next;
        }
    }
    sceneData->dirty = true;

}