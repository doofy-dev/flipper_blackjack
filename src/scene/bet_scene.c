#include <blackjack_icons.h>
#include "bet_scene.h"
#include "../game_state.h"
#include "../settings.h"
#include "../graphics/asset.h"
#include "../graphics/render.h"
#include "../math/equation.h"

uint8_t bet_mul = 0;
static Vector play_pos = (Vector){SCREEN_WIDTH - 6, 55};
static Buffer *icon_play, *icon_ok;

void bet_start(void* data, SceneData* sceneData)
{
    UNUSED(data);
    sceneData->dirty = true;
    icon_play = asset_get_icon(&I_play);
    icon_ok = asset_get_icon(&I_ok);
}

void bet_render(void* data, SceneData* sceneData)
{
    UNUSED(data);
    //draw boxes
    uint8_t w = 10;
    uint8_t h = 16;
    uint8_t t = 23;
    Vector pos = {75 - (bet_mul * w + 4 * bet_mul), 24};
    for (uint8_t i = 0; i < 3; i++)
    {
        uint8_t s = 45 + (w * i) + (4 * i);
        draw_rbox(sceneData->buffer, (Rect){s, t, w, h}, COLOR_BLACK, COLOR_TRANSPARENT);
    }

    RenderData render_data = {
        .poly = RECTANGLE(0, 0, icon_play->real_width, icon_play->height),
        .sprite = icon_play,
        .color = COLOR_BLACK
    };
    Transform transform= {
        .scale = {1,1},
        .dirty = true,
        .position = pos,
        .rotation = -90,
        .transformation_matrix = IDENTITY_MATRIX
    };
    compute_transformation_matrix(&transform, NULL);
    cache_position(&render_data.poly, &transform);
    rasterize(sceneData->buffer, &render_data);

    //draw arrows
    pos.x += 7;
    pos.y = 39;

    transform.position=pos;
    transform.rotation=90;
    transform.dirty = true;
    compute_transformation_matrix(&transform, NULL);
    cache_position(&render_data.poly, &transform);
    rasterize(sceneData->buffer, &render_data);


    render_data.sprite = icon_ok;
    render_data.poly = (Poly)RECTANGLE(0, 0, icon_ok->real_width, icon_ok->height);
    transform.position=play_pos;
    transform.rotation=0;
    transform.dirty = true;
    compute_transformation_matrix(&transform, NULL);
    cache_position(&render_data.poly, &transform);
    rasterize(sceneData->buffer, &render_data);
}

void bet_render_ui(void* data, SceneData* sceneData)
{
    GameState* state = (GameState*)data;
    UNUSED(sceneData);
    //draw numbers
    char score_cell[3];
    int hundreds = (int)(state->state->bet / 100);
    int tens = (int)((state->state->bet % 100) / 10);
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

void handle_bet(GameState* state, int8_t change)
{
    int16_t current = state->state->bet;
    int16_t next = (int16_t)(current + change);
    if (next > MAX_BET)
    {
        next = MAX_BET;
    }
    else if (next < 1)
    {
        next = 1;
    }
    state->state->bet = next;
}

void bet_input(void* data, SceneData* sceneData, InputKey key, InputType type)
{
    GameState* state = (GameState*)data;
    sceneData->dirty = true;
    int8_t change = (int8_t)pow(10, bet_mul);

    if (type == InputTypePress)
    {
        if (key == InputKeyOk)
        {
            //Store bet before continuing
            save_settings(state);
            sceneData->scene_switch = Next;
        }
        else if (key == InputKeyLeft)
        {
            if (bet_mul < 2) bet_mul++;
        }
        else if (key == InputKeyRight)
        {
            if (bet_mul > 0) bet_mul--;
        }
        else if (key == InputKeyUp)
        {
            handle_bet(state, change);
        }
        else if (key == InputKeyDown)
        {
            handle_bet(state, (int8_t)(-change));
        }
    }
}
