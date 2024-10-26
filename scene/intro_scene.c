#include "intro_scene.h"
#include "../game_state.h"
#include "../util/card.h"
#include "play_scene.h"

#define DECK_POS_X 64
#define DECK_POS_Y 6
//0: dealer, 1 player hand
static uint8_t target_hand = 0;
static CardAnimatorData animatorData;

void intro_start(void *data, SceneData *sceneData) {
    GameState *game_state = data;
    sceneData->dirty = true;

    //generate deck
    list_free(game_state->state->deck);
//    game_state->state->deck = deck_generate(1);
    game_state->state->deck = deck_generate(6);

    list_free_data(game_state->state->dealer);
    for (uint8_t i = 0; i < 4; i++) {
        list_free_data(game_state->state->hand[i]);
        game_state->state->doubled[i] = false;
    }
    game_state->state->insurance = false;
    game_state->state->hand_count = 1;
    game_state->state->current_hand = 0;


    animatorData.start_position = (Vector) {DECK_POS_X, DECK_POS_Y};
    animatorData.start_rotation = 0;
    animatorData.end_rotation = 0;
    animatorData.start_scale = (Vector) {1,1};
    animatorData.end_scale = (Vector){1,1};
    animatorData.finished = false;
    animatorData.state = 0;
    animatorData.card = NULL;


}

void intro_render(void *data, SceneData *sceneData) {
    UNUSED(data);
    buffer_set_sprite_rotation(0);
    card_render_back(DECK_POS_X, DECK_POS_Y, false, sceneData->buffer, 22);
    //call the play renderer
    play_render(data, sceneData);

    if (!animatorData.finished && animatorData.card) {
        buffer_set_transform(&(animatorData.transformMatrix));
        card_try_render(animatorData.card, 0, 0, false, sceneData->buffer, 22);
    }
    buffer_set_transform(NULL);
}

void intro_render_ui(void *data, SceneData *sceneData) {
    //call the play ui renderer
    UNUSED(data);
    UNUSED(sceneData);
    play_render_ui(data, sceneData);
}

void handle_placement(GameState *game_state) {
    if (target_hand == 0) {
        animatorData.card->exposed = game_state->state->dealer->count == 1;
        list_push_back(list_pop_back(game_state->state->deck), game_state->state->dealer);
    } else {
        animatorData.card->exposed = true;
        list_push_back(list_pop_back(game_state->state->deck), game_state->state->hand[0]);
    }
    animatorData.card = NULL;
}

void intro_update(void *data, SceneData *sceneData) {
    sceneData->dirty = true;
    GameState *game_state = data;

    if (game_state->state->dealer->count == 2 && game_state->state->hand[0]->count == 2) {
        animatorData.card = NULL;
        sceneData->scene_switch = Next;
    }

    if (animatorData.finished || animatorData.card == NULL) {
        animatorData.card = list_peek_back(game_state->state->deck);

        if (target_hand == 0) {

            if (game_state->state->dealer->count == 0)
                animatorData.end_position = (Vector) {10, 13};
            else {
                animatorData.end_scale.y=-1;
                animatorData.end_position = (Vector) {20, 18};
            }
        } else {
            animatorData.end_scale.y=-1;
            animatorData.end_position = (Vector) {117, 50};
        }
        animatorData.state = 0;
        animatorData.finished = false;
        card_compute_animation_state(&animatorData, 0, 0);
    } else {
        card_compute_animation_state(&animatorData, sceneData->delta_time, target_hand == 0 ? 2.0f : 2.5f);

        if(animatorData.state>0.5 && animatorData.end_scale.y<0){
            animatorData.card->exposed=true;
            animatorData.end_scale.y=1;
        }

        if (animatorData.finished) {
            handle_placement(game_state);
            target_hand = (target_hand + 1) % 2;
        }
    }

}

void intro_input(void *data, SceneData *sceneData, InputKey key, InputType type) {
    GameState *game_state = data;

    if (type == InputTypePress && key == InputKeyOk) {
        sceneData->scene_switch = Next;
        while (game_state->state->dealer->count < 2 || game_state->state->hand[0]->count < 2) {
            if (animatorData.card == NULL)
                animatorData.card = list_peek_back(game_state->state->deck);
            handle_placement(game_state);
            target_hand = (target_hand + 1) % 2;
        }
    }
}