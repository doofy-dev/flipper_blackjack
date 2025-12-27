#include "split_screen.h"
#include "../util/card.h"
#include "../game_state.h"

static CardAnimatorData animatorData[4];

#define SPLIT_ANIM_LENGTH 0.75f

static float timer = 0;
static Vector deck_start = {64,
                            6};
static Vector start[2] = {{SCREEN_WIDTH - 18, SCREEN_HEIGHT - 13},
                          {SCREEN_WIDTH - 10, SCREEN_HEIGHT - 13}};
static Vector target[4] = {
    {25,  40},
    {103, 40},
    {45,  40},
    {83,  40},
};
Tweener tweeners[4];

static float target_scale = 2.0f;
static bool outro_anim = false;

void split_start(void *data, SceneData *sceneData) {
    UNUSED(sceneData);
    timer = 0;
    GameState *game_state = (GameState *) data;
    outro_anim = false;
    int8_t target_hand = -1;
    for (int8_t i = 0; i < 4; i++) {
        if (game_state->state->hand[i]->count == 0) {
            list_push_back(list_pop_back(game_state->state->hand[game_state->state->current_hand]),
                           game_state->state->hand[i]);
            target_hand = i;
            game_state->state->hand_count++;
            break;
        }
    }

    if (target_hand >= 0) {
        Card *c = list_pop_back(game_state->state->deck);
        list_push_back(c, game_state->state->hand[game_state->state->current_hand]);

        c = list_pop_back(game_state->state->deck);

        list_push_back(c, game_state->state->hand[target_hand]);

        game_state->state->current_bet[target_hand] = game_state->state->bet;

    } else {
        FURI_LOG_W("SPLIT", "Can't place new card to an empty hand");
    }

    for (uint8_t _i = 0; _i < 4; _i++) {
        animatorData[_i].start_position = _i < 2 ? start[_i] : (Vector) {64, 11};
        animatorData[_i].end_position = target[_i];
        animatorData[_i].start_rotation = 0;//_i < 2 ? 0 : (_i == 3 ? 180 : -180);
        animatorData[_i].end_rotation = 0;
        animatorData[_i].start_scale = (Vector) {1, 1};
        animatorData[_i].flip = true;

        animatorData[_i].end_scale = (Vector) {target_scale, target_scale};
        if (_i > 1) animatorData[_i].end_scale.y *= -1;

        if (_i == 0)
            animatorData[_i].card = list_peek_index(game_state->state->hand[game_state->state->current_hand], 0);
        else if (_i == 1)
            animatorData[_i].card = list_peek_index(game_state->state->hand[target_hand], 0);
        else if (_i == 2)
            animatorData[_i].card = list_peek_index(game_state->state->hand[game_state->state->current_hand], 1);
        else if (_i == 3)
            animatorData[_i].card = list_peek_index(game_state->state->hand[target_hand], 1);

        /*
        tweeners[_i] = (Tweener) {
            .update=&card_compute_animation_state,
            .length=SPLIT_ANIM_LENGTH,
            .delay=_i < 2 ? 0 : (_i == 2 ? SPLIT_ANIM_LENGTH : SPLIT_ANIM_LENGTH * 2),
            .data=&(animatorData[_i])
        };

        tweener_start(&tweeners[_i]);*/
    }
}

void split_render(void *data, SceneData *sceneData) {
    UNUSED(data);
    UNUSED(sceneData);
    UNUSED(deck_start);
    /*GameState *game_state = (GameState *) data;
    buffer_set_sprite_rotation(0);
    if (!outro_anim && !game_state->state->tweeners[3].finished) {
        card_render_back((int16_t) deck_start.x, (int16_t) deck_start.y, false, sceneData->buffer, 22);
    }

    for (uint8_t i = 0; i < 4; i++) {
        buffer_set_transform(&(animatorData[i].transformMatrix));
        card_try_render(animatorData[i].card, 0, 0, false, sceneData->buffer, 22);

        if (!game_state->state->tweeners[i].finished && i > 1) break;
    }

    buffer_set_transform(NULL);*/

}

void split_update(void *data, SceneData *sceneData) {
    UNUSED(data);
    sceneData->dirty = true;

    //start the outro animation after the drawing finishes
    if (!tweeners[3].finished) return;

    timer += sceneData->delta_time;

    if (timer > 1 && !outro_anim) {
        outro_anim = true;
        CardAnimatorData tmp = animatorData[1];
        animatorData[1] = animatorData[2];
        animatorData[2] = tmp;

        //animate cards back to position
        for (uint8_t i = 0; i < 4; i++) {

            animatorData[i].start_scale = animatorData[i].end_scale;
            animatorData[i].start_rotation = animatorData[i].end_rotation;
            animatorData[i].start_position = animatorData[i].end_position;

            animatorData[i].end_rotation = 0;


            if (i > 1) {
                animatorData[i].end_position.x = i == 2 ? (SCREEN_WIDTH + 18) : (SCREEN_WIDTH + 36);
            } else {
                animatorData[i].end_scale = (Vector) {1, 1};
                animatorData[i].end_position = (Vector) {i == 0 ? SCREEN_WIDTH - 18 : SCREEN_WIDTH - 10, 50};
            }

            // tweeners[i].length = SPLIT_ANIM_LENGTH / 3.f;
            tweeners[i].delay = i > 1 ? 0 : SPLIT_ANIM_LENGTH / 3.f;

            tweener_start(&(tweeners[i]));
        }

        return;
    }

    if (outro_anim && tweeners[0].finished) {
        sceneData->scene_switch = Prev;
    }
}

void split_input(void *data, SceneData *sceneData, InputKey key, InputType type) {
    UNUSED(data);
    if (type == InputTypePress && key == InputKeyOk) {

        for (uint8_t i = 0; i < 4; ++i) {
            if (!tweeners[i].finished) {
                tweener_end(&(tweeners[i]));
            }
        }

        sceneData->scene_switch = Prev;
    }
}