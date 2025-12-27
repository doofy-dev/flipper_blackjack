#include "intro_scene.h"
#include "../game_state.h"
#include "../util/card.h"
#include "play_scene.h"
#include "../animation_data.h"
#include "../util/tweener.h"
#include "../math/equation.h"

#define DECK_POS_X 64
#define DECK_POS_Y 6
//0: dealer, 1 player hand
static uint8_t animation_index = 0;
const Matrix deck_transform = IDENTITY_MATRIX;

void intro_start(void* data, SceneData* sceneData)
{
    GameState* game_state = data;
    sceneData->dirty = true;
    animation_index = 0;

    //generate deck
    list_free(game_state->state->deck);
    game_state->state->deck = deck_generate(6);

    list_free_data(game_state->state->dealer);
    for (uint8_t i = 0; i < 4; i++)
    {
        list_free_data(game_state->state->hand[i]);
        game_state->state->doubled[i] = false;
        game_state->state->current_bet[i] = 0;
    }

    game_state->state->current_bet[0] = game_state->state->bet;

    game_state->state->insurance = false;
    game_state->state->hand_count = 1;
    game_state->state->current_hand = 0;

    game_state->animatorData[0] = (Tweener){
        .data = list_peek_back(game_state->state->deck),
        .update = &card_compute_animation_state,
        .animation_data = &INTRO_DEALING[0],
        .delay = 0
    };

    tweener_start(&game_state->animatorData[0]);
}

void intro_render(void* data, SceneData* sceneData)
{
    UNUSED(sceneData);
    UNUSED(data);
    GameState* game_state = data;
    Tweener* animationData = &game_state->animatorData[0];
    bool animating = !animationData->finished && animationData->data && animation_index < 4;

    if (animating)
    {
        set_card_flip_uv(FLIP_VERTICAL);
        card_render_back(DECK_POS_X, DECK_POS_Y, false, sceneData->buffer);
        set_card_flip_uv(FLIP_NONE);
    }

    deck_render(game_state->state->dealer, Covered, 10, 13, false, false, sceneData->buffer);

    deck_render(game_state->state->hand[0], PileUpsideDown, SCREEN_WIDTH - 10 - (animating ? 8 : 0), SCREEN_HEIGHT - 13,
                false, false, sceneData->buffer);

    if (!animationData->finished && animationData->data)
    {
        card_set_transform_parent(&animationData->transform_matrix);
        card_try_render(animationData->data, 0, 0, false, sceneData->buffer);
        card_set_transform_parent(NULL);
    }
}

void intro_render_ui(void* data, SceneData* sceneData)
{
    //call the play ui renderer
    UNUSED(data);
    UNUSED(sceneData);
    play_render_ui(data, sceneData);
}

void handle_placement(GameState* game_state)
{
    Card* card = game_state->animatorData[0].data;

    if (animation_index % 2 == 0)
    {
        card->exposed = game_state->state->dealer->count == 1;
        list_push_back(list_pop_back(game_state->state->deck), game_state->state->dealer);
    }
    else
    {
        card->exposed = true;
        list_push_back(list_pop_back(game_state->state->deck), game_state->state->hand[0]);
    }

    game_state->animatorData[0].data = NULL;

}

void intro_update(void* data, SceneData* sceneData)
{
    sceneData->dirty = true;
    GameState* game_state = data;
    Tweener* animationData = &game_state->animatorData[0];

    //to avoid double processing the animated card
    if (sceneData->scene_switch == Next) return;

    if (animationData->finished && animation_index<4)
    {
        handle_placement(game_state);
        animation_index++;

        if (game_state->state->dealer->count == 2 && game_state->state->hand[0]->count == 2)
        {
            // sceneData->scene_switch = Next;
            //early exit if we have enough card
            return;
        }

        animationData->data = list_peek_back(game_state->state->deck);

        //since the first card is not initiated here, the rest could be -1
        animationData->animation_data = &INTRO_DEALING[animation_index];
        tweener_start(animationData);
    }
}

void intro_input(void* data, SceneData* sceneData, InputKey key, InputType type)
{
    UNUSED(sceneData);
    GameState* game_state = data;
    Tweener* animationData = &game_state->animatorData[0];

    if (type == InputTypePress && key == InputKeyOk)
    {
        // sceneData->scene_switch = Next;

        if (!animationData->finished)
        {
            tweener_end(animationData);
        }

        while (game_state->state->dealer->count < 2 || game_state->state->hand[0]->count < 2)
        {
            if (animationData->data == NULL)
                animationData->data = list_peek_back(game_state->state->deck);
            handle_placement(game_state);
            animation_index++;
        }
    }
}
