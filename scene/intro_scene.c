#include "intro_scene.h"
#include "../game_state.h"
#include "../util/card.h"
#include "play_scene.h"
#include "../util/tweener.h"

#define DECK_POS_X 64
#define DECK_POS_Y 6
//0: dealer, 1 player hand
static uint8_t target_hand = 0;
static CardAnimatorData animatorData={};


void intro_start(void *data, SceneData *sceneData) {
    GameState *game_state = data;
    sceneData->dirty = true;

    //generate deck
    list_free(game_state->state->deck);
    game_state->state->deck = deck_generate(6);

    list_free_data(game_state->state->dealer);
    for (uint8_t i = 0; i < 4; i++) {
        list_free_data(game_state->state->hand[i]);
        game_state->state->doubled[i] = false;
        game_state->state->current_bet[i] = 0;
    }

    game_state->state->current_bet[0] = game_state->state->bet;

    game_state->state->insurance = false;
    game_state->state->hand_count = 1;
    game_state->state->current_hand = 0;


    animatorData.start_position = (Vector) {DECK_POS_X, DECK_POS_Y};
    animatorData.flip = true;
    animatorData.start_rotation = 0;
    animatorData.end_rotation = 0;
    animatorData.end_position = (Vector) {10, 13};
    animatorData.start_scale = (Vector) {1, 1};
    animatorData.end_scale = (Vector) {1, 1};
    animatorData.card = list_peek_back(game_state->state->deck);

    game_state->state->tweeners[0]=(Tweener){
        .update=&card_compute_animation_state,
        .length=0.75f,
        .delay=0,
        .data=&animatorData
    };

    tweener_start(&(game_state->state->tweeners[0]));
}

void intro_render(void *data, SceneData *sceneData) {
    GameState *game_state = data;
    buffer_set_sprite_rotation(0);
    card_render_back(DECK_POS_X, DECK_POS_Y, false, sceneData->buffer, 22);

    deck_render(game_state->state->dealer, Covered, 10, 13, false, false, sceneData->buffer);
    bool animating=!game_state->state->tweeners[0].finished && animatorData.card && target_hand==1;
    deck_render(game_state->state->hand[0], PileUpsideDown, SCREEN_WIDTH - 10 - (animating? 8 : 0), SCREEN_HEIGHT - 13,
                false, false, sceneData->buffer);

    if (!game_state->state->tweeners[0].finished && animatorData.card) {
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

    //to avoid double processing the animated card
    if (sceneData->scene_switch == Next) return;

    if (game_state->state->tweeners[0].finished) {
        handle_placement(game_state);
        target_hand = (target_hand + 1) % 2;
    }

    if (game_state->state->dealer->count == 2 && game_state->state->hand[0]->count == 2) {
        animatorData.card = NULL;
        sceneData->scene_switch = Next;

        //early exit if we have enough card
        return;
    }

    if (game_state->state->tweeners[0].finished) {
        animatorData.card = list_peek_back(game_state->state->deck);

        //since the first card is not initiated here, the rest could be -1
        animatorData.end_scale.y = -1;
        animatorData.start_scale.y = 1;

        if (target_hand == 0) {
            animatorData.end_position = (Vector) {20, 18};
        } else {
            animatorData.end_position = (Vector) {117, 50};
        }

        game_state->state->tweeners[0] = (Tweener) {
            .data=&animatorData,
            .update=&card_compute_animation_state,
            .length=target_hand == 0 ? 0.75f : 1.f,
            .delay=0
        };
        tweener_start(&(game_state->state->tweeners[0]));
    }

}

void intro_input(void *data, SceneData *sceneData, InputKey key, InputType type) {
    GameState *game_state = data;

    if (type == InputTypePress && key == InputKeyOk) {
        sceneData->scene_switch = Next;
        if(!game_state->state->tweeners[0].finished) {
            tweener_end(&(game_state->state->tweeners[0]));
        }

        while (game_state->state->dealer->count < 2 || game_state->state->hand[0]->count < 2) {
            if (animatorData.card == NULL)
                animatorData.card = list_peek_back(game_state->state->deck);
            handle_placement(game_state);
            target_hand = (target_hand + 1) % 2;
        }
    }
}