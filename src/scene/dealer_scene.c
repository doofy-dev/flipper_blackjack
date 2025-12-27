#include "dealer_screen.h"
#include "play_scene.h"

Matrix endMatrix;
bool initial_done = false;
/*Tweener flip_tweener, dealer_tweener;
static CardAnimatorData dealer_anim_data = {
    .start_position=(Vector) {10, 13},
    .flip=true,
    .start_rotation=0,
    .end_rotation=0,
    .end_position=(Vector) {117, 50},
    .end_scale=(Vector) {1, -1},
    .start_scale=(Vector) {1, 1},
    .card=NULL
};*/
/*static CardAnimatorData flip_anim_data = {
    .start_position=(Vector) {64, 6},
    .flip=true,
    .start_rotation=0,
    .end_rotation=0,
    .end_position=(Vector) {117, 50},
    .end_scale=(Vector) {1, -1},
    .start_scale=(Vector) {1, 1},
    .card=NULL
};*/

void dealer_start(void *data, SceneData *sceneData) {
//animate dealer hand to the center, flip the first card animated, then draw cards animated

    UNUSED(data);
    UNUSED(sceneData);
    /*
    flip_tweener = (Tweener) {
        .update=&card_compute_animation_state,
        .length=1,
        .data=&dealer_anim
    };

    dealer_tweener = (Tweener) {
        .update=&card_compute_animation_state,
        .length=1,
        .data=&flip_anim,
        .delay=1
    };
    */

    // GameState *state = (GameState *) data;
    /*state->state->tweeners[0] = (Tweener) {
        .update=&card_compute_animation_state,
        .length=1,
        .data=&dealer_anim
    };
    state->state->tweeners[1] = (Tweener) {
        .update=&card_compute_animation_state,
        .length=1,
        .data=&flip_anim,
        .delay=1
    };*/
}

//called at the end of animation
void draw_card(GameState *gameState, SceneData *sceneData){
    UNUSED(gameState);
    UNUSED(sceneData);
}

void dealer_render(void *data, SceneData *sceneData) {
    GameState *state = (GameState *) data;
    UNUSED(sceneData);UNUSED(state);
    /*buffer_set_sprite_rotation(0);

    if (initial_done) {
        buffer_set_transform(&endMatrix);
        deck_render(state->state->dealer, Pile, 10, 13, false, false, sceneData->buffer);
    }*/
}

void dealer_render_ui(void *data, SceneData *sceneData) {
    play_render_ui(data, sceneData);
    UNUSED(data);
    UNUSED(sceneData);
}

void dealer_update(void *data, SceneData *sceneData) {
    UNUSED(data);
    UNUSED(sceneData);
    // GameState *state = (GameState *) data;

    /*if (state->state->tweeners[0].finished){
        if(initial_done){

        }else{
            ((Card*)state->state->dealer->head->data)->exposed=true;
            matrix_copy(&(dealer_anim.transformMatrix), &endMatrix);
            initial_done=true;
        }
    }*/
}

void dealer_input(void *data, SceneData *sceneData, InputKey key, InputType type) {
    UNUSED(data);
    UNUSED(sceneData);
    UNUSED(key);
    UNUSED(type);

}