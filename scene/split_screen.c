#include "split_screen.h"
#include "../util/card.h"
#include "../game_state.h"

static CardAnimatorData animatorData[4];

static float timer = 0;
static Vector deck_start = {64,
                            6};
static Vector start[4] = {{SCREEN_WIDTH - 18, SCREEN_HEIGHT - 13},
                          {SCREEN_WIDTH - 10, SCREEN_HEIGHT - 13}};
static Vector target[4] = {
    {25,  40},
    {103, 40},
    {45,  40},
    {83,  40},
};


static float target_scale = 2.0f;
static bool initial_anim_complete = false;
static bool outro_anim = false;

void split_start(void *data, SceneData *sceneData) {
    UNUSED(sceneData);
    timer = 0;
    GameState *state = (GameState *) data;
    initial_anim_complete = false;
    outro_anim = false;
    int8_t target_hand = -1;
    for (int8_t i = 0; i < 4; i++) {
        if (state->state->hand[i]->count == 0) {
            list_push_back(list_pop_back(state->state->hand[state->state->current_hand]), state->state->hand[i]);
            target_hand = i;
            state->state->hand_count++;
            break;
        }
    }

    if (target_hand >= 0) {
        Card *c = list_pop_back(state->state->deck);
        list_push_back(c, state->state->hand[state->state->current_hand]);

        c = list_pop_back(state->state->deck);

        list_push_back(c, state->state->hand[target_hand]);

    } else {
        FURI_LOG_W("SPLIT", "Can't place new card to an empty hand");
    }

    for (uint8_t _i = 0; _i < 4; _i++) {


        animatorData[_i].start_position = _i < 2 ? start[_i] : (Vector) {64, 11};
        animatorData[_i].end_position = target[_i];
        animatorData[_i].start_rotation = 0;//_i < 2 ? 0 : (_i == 3 ? 180 : -180);
        animatorData[_i].end_rotation = 0;
        animatorData[_i].start_scale = (Vector) {1, 1};
        animatorData[_i].end_scale =
            _i > 1 ? (Vector) {target_scale, -target_scale} : (Vector) {target_scale, target_scale};
        animatorData[_i].finished = false;
        animatorData[_i].state = 0;

        if (_i == 0)
            animatorData[_i].card = list_peek_index(state->state->hand[state->state->current_hand], 0);
        else if (_i == 1)
            animatorData[_i].card = list_peek_index(state->state->hand[target_hand], 0);
        else if (_i == 2)
            animatorData[_i].card = list_peek_index(state->state->hand[state->state->current_hand], 1);
        else if (_i == 3)
            animatorData[_i].card = list_peek_index(state->state->hand[target_hand], 1);

        card_compute_animation_state(&(animatorData[_i]), 0, 0);
    }


}

void split_render(void *data, SceneData *sceneData) {
    UNUSED(data);
    buffer_set_sprite_rotation(0);
    if (animatorData[3].finished == false) {
        card_render_back((int16_t) deck_start.x, (int16_t) deck_start.y, false, sceneData->buffer, 22);
    }
    uint8_t end = !initial_anim_complete ? 2 : (!animatorData[2].finished ? 3 : 4);

    for (uint8_t i = 0; i < end; i++) {
        buffer_set_transform(&(animatorData[i].transformMatrix));
        card_try_render(animatorData[i].card, 0, 0, false, sceneData->buffer, 22);
    }

    if (initial_anim_complete) {
        Vector v;
        matrix_get_translation(&(animatorData[2].transformMatrix), &v);
    }

    buffer_set_transform(NULL);

}

void split_update(void *data, SceneData *sceneData) {
    UNUSED(data);
    initial_anim_complete = animatorData[0].finished == true && animatorData[1].finished == true;

    if(!outro_anim) {
        for (uint8_t i = 0; i < 2; i++) {
            if (animatorData[i].finished == false) {
                card_compute_animation_state(&(animatorData[i]), sceneData->delta_time, 2);
                sceneData->dirty = true;
            }
        }
        if ((initial_anim_complete && animatorData[3].finished == false)) {
            for (uint8_t i = 2; i < 4; i++) {
                if (animatorData[i].finished == false) {
                    card_compute_animation_state(&(animatorData[i]), sceneData->delta_time, 2);
                    sceneData->dirty = true;
                    if (animatorData[i].state >= 0.5f) {
                        animatorData[i].card->exposed = true;
                        animatorData[i].end_scale.y = target_scale;
                    }
                    break;
                }
            }
        }
    }else{

        for (uint8_t i = 0; i < 4; i++) {
            if (animatorData[i].finished == false) {
                card_compute_animation_state(&(animatorData[i]), sceneData->delta_time, 2);
                sceneData->dirty = true;
            }
        }
    }

    timer+=sceneData->delta_time;

    if (animatorData[3].finished && timer > 3 && !outro_anim) {
        outro_anim = true;
        CardAnimatorData tmp = animatorData[1];
        animatorData[1] = animatorData[2];
        animatorData[2]=tmp;

        //animate cards back to position
        for (uint8_t i = 0; i < 2; i++) {
            animatorData[i].start_scale = animatorData[i].end_scale;
            animatorData[i].end_scale = (Vector) {1, 1};
            animatorData[i].start_rotation = animatorData[i].end_rotation;
            animatorData[i].end_rotation = 0;
            animatorData[i].state = 0;
            animatorData[i].finished = false;
            animatorData[i].start_position = animatorData[i].end_position;
            animatorData[i].end_position = (Vector) {i==0 ? SCREEN_WIDTH- 18 : SCREEN_WIDTH - 10, 50};
        }

        for (uint8_t i = 2; i < 4; i++) {
            animatorData[i].start_scale = animatorData[i].end_scale;
            animatorData[i].end_scale = (Vector){1,1};
            animatorData[i].start_rotation = animatorData[i].end_rotation;
            animatorData[i].state = 0;
            animatorData[i].finished = false;
            animatorData[i].start_position = animatorData[i].end_position;
            animatorData[i].end_position = (Vector) {300, 32};
        }
    }
    if(outro_anim && animatorData[3].finished){
        sceneData->scene_switch = Prev;
    }
}

void split_input(void *data, SceneData *sceneData, InputKey key, InputType type) {
    UNUSED(data);
    if (type == InputTypePress && key == InputKeyOk) {
        sceneData->scene_switch = Prev;
    }
}