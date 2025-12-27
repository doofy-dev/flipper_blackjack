#include <notification/notification_messages.h>
#include "main_scene.h"
#include "../game_state.h"
#include "../settings.h"
#include "../math/equation.h"
#include "../graphics/render.h"
#include "../graphics/buffer.h"
#include "blackjack_icons.h"
#include "../graphics/asset.h"
#include "../util/helpers.h"
#include "../util/audio.h"
#include "../sounds.h"

static Vector logo_pos = (Vector) {25, 18};
static Vector cards_pos = (Vector) {95, 15};
static Vector speaker_pos = (Vector) {13, 53};
static Vector left_arrow_pos = (Vector) {6, 53};
static Vector play_pos = (Vector) {SCREEN_WIDTH - 6, 55};

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
    set_audio(&bg_music);
    if (state->sound_enabled)
        play_audio();
}

void main_render(void *data, SceneData *sceneData) {
    Transform transform= {
        .scale = {1,1},
        .dirty = true,
        .position = cards_pos,
        .rotation = 0,
        .transformation_matrix = IDENTITY_MATRIX
    };

    RenderData render_data = {
        .poly = RECTANGLE(0, 0, icon_main->real_width,icon_main->height),
        .sprite = icon_main,
        .tile_mode = TILE_NONE,
        .color = COLOR_BLACK,
        .mask_color = COLOR_WHITE
    };

    compute_transformation_matrix(&transform, NULL);
    cache_position(&render_data.poly, &transform);
    rasterize(sceneData->buffer, &render_data);

    transform.dirty = true;
    transform.position = logo_pos;
    render_data.sprite = icon_logo;
    compute_transformation_matrix(&transform, NULL);
    render_data.poly = (Poly)RECTANGLE(0, 0, icon_logo->real_width,icon_logo->height);
    cache_position(&render_data.poly, &transform);
    rasterize(sceneData->buffer, &render_data);

    GameState *state = (GameState *) data;
    Buffer *sprite = icon_speaker_off;

    if (state->sound_enabled)
    {
        sprite = icon_speaker_on;
    }

    transform.dirty = true;
    transform.position = speaker_pos;
    render_data.sprite = sprite;
    render_data.poly = (Poly)RECTANGLE(0, 0, render_data.sprite->real_width, render_data.sprite->height);
    compute_transformation_matrix(&transform, NULL);
    cache_position(&render_data.poly, &transform);
    rasterize(sceneData->buffer, &render_data);

    transform.dirty = true;
    transform.position = left_arrow_pos;
    render_data.sprite = icon_play;
    render_data.poly = (Poly)RECTANGLE(0, 0, render_data.sprite->real_width, render_data.sprite->height);
    compute_transformation_matrix(&transform, NULL);
    flip_uv(&render_data.poly, FLIP_HORIZONTAL);
    cache_position(&render_data.poly, &transform);
    rasterize(sceneData->buffer, &render_data);

    transform.dirty = true;
    transform.position = play_pos;
    render_data.sprite = icon_ok;
    render_data.poly = (Poly)RECTANGLE(0, 0, render_data.sprite->real_width, render_data.sprite->height);
    compute_transformation_matrix(&transform, NULL);
    cache_position(&render_data.poly, &transform);
    rasterize(sceneData->buffer, &render_data);
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
            stop_audio();
            sceneData->scene_switch = Next;
        }
    }
    sceneData->dirty = true;

}