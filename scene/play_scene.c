#include <blackjack_icons.h>
#include "play_scene.h"
#include "../game_state.h"
#include "../scene_setup.h"
#include "../util/helpers.h"
#include "../util/asset.h"

typedef enum {
    Hit,
    Stand,
    DoubleDown,
    Split,
    Insurance
} MenuItems;

const char *menu_items[5] = {
    "Hit", "Stand", "Double", "Split", "Insurance"
};

bool can_insure = false;
static MenuItems selectedMenu;
static char str[10];
static Buffer *icon_play;

static bool is_doubling = false;
static bool is_hitting = false;
static bool swap_hand = false;

void play_start(void *data, SceneData *sceneData) {
    GameState *state = (GameState *) data;
    UNUSED(sceneData);

    selectedMenu = Hit;
    Card *c = list_peek_index(state->state->dealer, 1);

    //If the dealer has Ace as the visible card, player can insure the round
    can_insure = state->state->dealer->count == 2 && c && c->value == ACE;
    is_doubling = false;
    is_hitting = false;
    swap_hand = false;
    icon_play = asset_get_icon(&I_play);
}


void play_render(void *data, SceneData *sceneData) {
    GameState *state = (GameState *) data;
    UNUSED(data);
    buffer_set_sprite_rotation(0);

    //render dealer hand
    if (state->state->dealer->count < 3) {
        deck_render(state->state->dealer, Covered, 10, 13, false, false, sceneData->buffer);
    } else {
        deck_render(state->state->dealer, Pile, 10, 13, false, false, sceneData->buffer);
    }

/* TODO: maybe do not render the play screen as a whole on split screen
    //do not render player hand and menu when we split
    if(sceneData->current_scene && sceneData->current_scene->data == &split_screen)
        return;*/

    //render player hand
    deck_render(state->state->hand[state->state->current_hand], PileUpsideDown, SCREEN_WIDTH - 10, SCREEN_HEIGHT - 13,
                false, false, sceneData->buffer);

    //draw arrows for menu
    if (sceneData->current_scene && sceneData->current_scene->data == &play_screen) {
        buffer_set_sprite_rotation(-90);
        Vector pos = (Vector) {25, SCREEN_HEIGHT - 23};
        buffer_draw_all(sceneData->buffer, icon_play, &pos);
        pos.y += 16;

        buffer_set_sprite_rotation(90);
        buffer_draw_all(sceneData->buffer, (Buffer *) icon_play, &pos);
    }
}

void play_render_ui(void *data, SceneData *sceneData) {
    GameState *game_state = data;
    //draw player coins, bet and hand counter
    char *score = money_formatter(game_state->state->balance, MAX_SCORE, game_state->state->balance_mul);
    canvas_set_font(sceneData->canvas, FontSecondary);
    canvas_set_color(sceneData->canvas, ColorBlack);
    canvas_draw_str_aligned(sceneData->canvas, SCREEN_WIDTH, 5, AlignRight, AlignCenter, "Balance:");
    canvas_draw_str_aligned(sceneData->canvas, SCREEN_WIDTH, 15, AlignRight, AlignCenter, score);

    free(score);


    //subtract (bet * hand_count) + insurance + double(max 4X)
    uint8_t val = hand_value(game_state->state->hand[game_state->state->current_hand], 21);

    snprintf(str, sizeof(str), "%d", val);

    canvas_set_color(sceneData->canvas, ColorBlack);
    canvas_draw_rbox(sceneData->canvas, SCREEN_WIDTH - 13, SCREEN_HEIGHT - 11, 14, 12, 2);
    canvas_set_color(sceneData->canvas, ColorWhite);
    canvas_draw_str_aligned(sceneData->canvas, SCREEN_WIDTH - 7, SCREEN_HEIGHT - 5, AlignCenter, AlignCenter, str);

    //if we are on our scene, draw the menu too
    if (sceneData->current_scene && sceneData->current_scene->data == &play_screen) {
        canvas_set_color(sceneData->canvas, ColorBlack);
        canvas_draw_rframe(sceneData->canvas, 0, SCREEN_HEIGHT - 20, 50, 11, 2);
        canvas_draw_str_aligned(sceneData->canvas, 25, SCREEN_HEIGHT - 14, AlignCenter, AlignCenter,
                                menu_items[selectedMenu]);

    }
}

void play_update(void *data, SceneData *sceneData) {
    UNUSED(sceneData);
    UNUSED(data);

    if (is_doubling) {

    } else {

    }

}

void next_menu(GameState *game_state) {
    selectedMenu = (selectedMenu + 1) % (Insurance + 1);
    if (
        (selectedMenu == Insurance && !can_insure) ||

        (selectedMenu == DoubleDown && !can_double(game_state)) ||
        (selectedMenu == Split && !can_split(game_state, game_state->state->hand[game_state->state->current_hand]))
        )
        next_menu(game_state);
}

void prev_menu(GameState *game_state) {
    selectedMenu = selectedMenu > 0 ? (selectedMenu - 1) : Insurance;
    if (
        (selectedMenu == Insurance && !can_insure) ||

        (selectedMenu == DoubleDown && !can_double(game_state)) ||
        (selectedMenu == Split && !can_split(game_state, game_state->state->hand[game_state->state->current_hand]))
        )
        prev_menu(game_state);
}

void play_input(void *data, SceneData *sceneData, InputKey key, InputType type) {
    GameState *game_state = data;
    sceneData->dirty = true;

    if (type == InputTypePress) {
        if (key == InputKeyUp) {
            prev_menu(data);
        } else if (key == InputKeyDown) {
            next_menu(data);
        } else if (key == InputKeyOk) {
            if (selectedMenu == DoubleDown) {
                is_doubling = true;
            } else if (selectedMenu == Hit) {
                is_hitting = true;
            } else if (selectedMenu == Stand) {
                if (game_state->state->current_hand < (game_state->state->hand_count - 1)) {
                    game_state->state->current_hand++;
                    swap_hand = true;
                    //animate hand transition
                } else {
                    //move to the dealer scene
                    sceneData->scene_switch = Index;
                    sceneData->scene_index = 5;
                }
            } else if (selectedMenu == Split) {
                sceneData->scene_switch = Next; // move to split screen
            }
        }
    }
}

//Only allow split if you have the same value cards and have less than 4 hands
bool can_split(GameState *game_state, List *hand) {
    if (game_state->state->hand_count < 4 && hand->count == 2) {
        Card *a = list_peek_front(hand);
        Card *b = list_peek_back(hand);

        uint8_t val_a = a->value == ACE ? 11 : MIN(a->value + 2, 10);
        uint8_t val_b = b->value == ACE ? 11 : MIN(b->value + 2, 10);

        return val_a == val_b;
    }

    return false;
}

//Only allow if you have the initial 2 card, without split
bool can_double(GameState *game_state) {
    return !game_state->state->doubled[game_state->state->current_hand] &&
           game_state->state->hand[game_state->state->current_hand]->count == 2 &&
           game_state->state->hand_count == 1;
}
