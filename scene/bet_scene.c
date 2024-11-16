#include <blackjack_icons.h>
#include "bet_scene.h"
#include "../game_state.h"
#include "../settings.h"
#include "../util/asset.h"

uint8_t bet_mul = 0;
static Vector play_pos = (Vector) {SCREEN_WIDTH - 4, 58};
static Buffer *icon_play, *icon_ok;

void bet_start(void *data, SceneData *sceneData) {
    UNUSED(data);
    sceneData->dirty = true;
    icon_play = asset_get_icon(&I_play);
    icon_ok = asset_get_icon(&I_ok);
}

void bet_render(void *data, SceneData *sceneData) {
    UNUSED(data);
    UNUSED(sceneData);
    //draw boxes
    Vector pos = {78, 20};
    uint8_t w = 10;
    uint8_t h = 16;
    uint8_t t = 24;
    for (uint8_t i = 0; i < 3; i++) {
        uint8_t s = 45 + (w * i) + (4*i);
        buffer_draw_rbox_frame(sceneData->buffer, s, t, s + w, t + h, Black);
    }


    //draw arrows
    pos = (Vector) {78 - 14 * bet_mul, 21};
    buffer_set_sprite_rotation(-90);
    buffer_draw_all(sceneData->buffer, icon_play, &pos);
    pos.y = 43;
    buffer_set_sprite_rotation(90);
    buffer_draw_all(sceneData->buffer, icon_play, &pos);
    buffer_set_sprite_rotation(0);
    buffer_draw_all(sceneData->buffer, icon_ok, &play_pos);
}

void bet_render_ui(void *data, SceneData *sceneData) {
    GameState *state = (GameState *) data;
    UNUSED(sceneData);
    //draw numbers
    char score_cell[3];
    int hundreds = (int) (state->state->bet / 100);
    int tens = (int) ((state->state->bet % 100) / 10);
    int ones = state->state->bet % 10;
    canvas_set_color(sceneData->canvas, ColorXOR);
    canvas_set_font(sceneData->canvas, FontPrimary);
    canvas_draw_str_aligned(state->canvas, 64, 10, AlignCenter, AlignCenter, "Place your bet");

    snprintf(score_cell, sizeof(score_cell), "%1d", hundreds);
    canvas_draw_str_aligned(state->canvas, 50, 32, AlignCenter, AlignCenter, score_cell);

    snprintf(score_cell, sizeof(score_cell), "%1d", tens);
    canvas_draw_str_aligned(state->canvas, 64, 32, AlignCenter, AlignCenter, score_cell);

    snprintf(score_cell, sizeof(score_cell), "%1d", ones);
    canvas_draw_str_aligned(state->canvas, 78, 32, AlignCenter, AlignCenter, score_cell);

    canvas_set_font(sceneData->canvas, FontSecondary);
    canvas_draw_str(sceneData->canvas, 104, 61, "Bet");
}

void handle_bet(GameState *state, int8_t change) {
    int16_t current = state->state->bet;
    int16_t next = (int16_t) (current + change);
    if (next > MAX_BET) {
        next = MAX_BET;
    } else if (next < 1) {
        next = 1;
    }
    state->state->bet = next;
}

void bet_input(void *data, SceneData *sceneData, InputKey key, InputType type) {
    GameState *state = (GameState *) data;
    sceneData->dirty = true;
    int8_t change = (int8_t) pow(10, bet_mul);

    if (type == InputTypePress) {
        if (key == InputKeyOk) {
            //Store bet before continuing
            save_settings(state);
            sceneData->scene_switch = Next;
        } else if (key == InputKeyLeft) {
            if (bet_mul < 2) bet_mul++;
        } else if (key == InputKeyRight) {
            if (bet_mul > 0) bet_mul--;
        } else if (key == InputKeyUp) {
            handle_bet(state, change);
        } else if (key == InputKeyDown) {
            handle_bet(state, (int8_t) (-change));
        }
    }

}