#include <blackjack_icons.h>
#include "play_scene.h"
#include "../scene_setup.h"
#include "../util/helpers.h"
#include "../math//equation.h"
#include "../graphics/asset.h"
#include "../util/scheduler.h"

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
Tweener animator_tweener;

static Scheduler change_hand_schedule = {
    .callback=&change_active_hand,
    .repeat=false,
    .timeout=2
};

static bool animated = false;



/*
static PackedTransform startTransform = {
    .position = (Vector){64,6},
    .scale = (Vector){1,-1},
    .rotation = 0
};
static PackedTransform endTransform;
static PackedTransform transformData;

static CardAnimatorData animatorData = {
    .flip=true,

    .start_rotation=0,
    .end_rotation=0,

    .start_position=(Vector) {64, 6},
    .end_position=(Vector) {117, 50},

    .start_scale=(Vector) {1, 1},
    .end_scale=(Vector) {1, -1},

    .card=NULL,
};
*/


void play_start(void *data, SceneData *sceneData) {
    GameState *state = (GameState *) data;
    UNUSED(sceneData);

    change_hand_schedule.data=data;
    selectedMenu = Hit;
    Card *c = list_peek_index(state->state->dealer, 1);

    //If the dealer has Ace as the visible card, player can insure the round
    can_insure = state->state->dealer->count == 2 && c && c->value == ACE;
    animated = false;
    icon_play = asset_get_icon(&I_play);
    check_menu(state, true);
}

void play_render(void *data, SceneData *sceneData) {
    UNUSED(sceneData);UNUSED(data);
    /*
    GameState *state = (GameState *) data;
    UNUSED(data);

    //render dealer hand
    if (state->state->dealer->count < 3) {
        deck_render(state->state->dealer, Covered, 10, 13, false, false, sceneData->buffer);
    } else {
        deck_render(state->state->dealer, Pile, 10, 13, false, false, sceneData->buffer);
    }

    bool animating = !animator_tweener.finished && animatorData.card;
    bool c = state->state->hand[state->state->current_hand]->count < 6;
    bool c2 = state->state->hand[state->state->current_hand]->count % 6 == 0;

    //render player hand
    deck_render(state->state->hand[state->state->current_hand], PileUpsideDown,
                SCREEN_WIDTH - 10 - (animating && c ? 8 : 0), SCREEN_HEIGHT - 13 - (animating && !c && c2 ? 9 : 0),
                false, false, sceneData->buffer);

    if (!sceneData->current_scene || sceneData->current_scene->data != &play_screen)
        return;

    //render drawing animation
    if (animating) {
        card_render_back(64, 6, false, sceneData->buffer);

        card_set_transform_parent(&(animatorData.transformMatrix));
        card_try_render(animatorData.card, 0, 0, false, sceneData->buffer);
        card_set_transform_parent(NULL);
    }
    */

    //draw arrows for menu
    if (!animated) {
        /*buffer_set_sprite_rotation(-90);
        Vector pos = (Vector) {25, SCREEN_HEIGHT - 23};
        buffer_draw_all(sceneData->buffer, icon_play, &pos);
        pos.y += 16;

        buffer_set_sprite_rotation(90);
        buffer_draw_all(sceneData->buffer, (Buffer *) icon_play, &pos);*/
    }
}

void play_render_ui(void *data, SceneData *sceneData) {
    UNUSED(data);
    UNUSED(sceneData);
    UNUSED(str);

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
    if (!animated && sceneData->current_scene && sceneData->current_scene->data == &play_screen) {
        canvas_set_color(sceneData->canvas, ColorBlack);
        canvas_draw_rframe(sceneData->canvas, 0, SCREEN_HEIGHT - 20, 50, 11, 2);
        canvas_draw_str_aligned(sceneData->canvas, 25, SCREEN_HEIGHT - 14, AlignCenter, AlignCenter,
                                menu_items[selectedMenu]);
    }
}

void change_active_hand(void *data, SceneData *sceneData) {
    GameState *game_state = data;
    //TODO: display message if bust. Stick around for a small amount of time
    //if we have more hands animate it, otherwise change scenes
    if (game_state->state->current_hand < (game_state->state->hand_count - 1)) {

    } else {
        animated = false;
        sceneData->scene_switch = Index;
        sceneData->scene_index = 5;
    }
}

void play_update(void *data, SceneData *sceneData) {
    UNUSED(sceneData);
    UNUSED(data);
    // GameState *game_state = data;
    sceneData->dirty = sceneData->dirty || animated;

    /*
    if (animated && animator_tweener.finished) {
        list_push_back(list_pop_back(game_state->state->deck),
                       game_state->state->hand[game_state->state->current_hand]);
        animatorData.card = NULL;
        animated = false;

        //if doubled or went over, swap hands
        if (game_state->state->doubled[game_state->state->current_hand] ||
            hand_value(game_state->state->hand[game_state->state->current_hand], 21) >= 21) {
            animated = true;
            scheduler_start(&change_hand_schedule);
        }
        check_menu(game_state, true);
    }*/
}

void next_menu(GameState *game_state) {
    selectedMenu = (selectedMenu + 1) % (Insurance + 1);
    check_menu(game_state, true);
}

void prev_menu(GameState *game_state) {
    selectedMenu = selectedMenu > 0 ? (selectedMenu - 1) : Insurance;
    check_menu(game_state, false);
}

void check_menu(GameState *game_state, bool toNext) {
    if (
        (selectedMenu == Insurance && !can_insure) ||
        (selectedMenu == Hit && hand_value(game_state->state->hand[game_state->state->current_hand], 21) >= 21) ||

        (selectedMenu == DoubleDown && !can_double(game_state)) ||
        (selectedMenu == Split && !can_split(game_state, game_state->state->hand[game_state->state->current_hand]))
        ) {
        if (toNext) {
            next_menu(game_state);
        } else {
            prev_menu(game_state);
        }
    }
}

void draw(GameState *state) {
    UNUSED(state);
    //add card and start animation
    /*animatorData.card = list_peek_back(state->state->deck);
    uint8_t id = state->state->hand[state->state->current_hand]->count;
    if (state->state->hand[state->state->current_hand]->count >= 6) {
        uint8_t id2 = (int8_t) id / 6;
        animatorData.end_position.x = SCREEN_WIDTH - 10 + (id % 6) * 8 + (id2 * 4) - 6 * 8;
        if (id >= 6) animatorData.end_position.x += 8;
    }
    animatorData.start_scale = (Vector) {1, 1};
    animatorData.end_scale = (Vector) {1, -1};
    animator_tweener = (Tweener) {
        .delay=0,
        .length=1.f,
        .update=&card_compute_animation_state,
        .data=&animatorData
    };

    tweener_start(&(animator_tweener));*/
    animated = true;
}

void doubleDown(GameState *state) {
    state->state->doubled[state->state->current_hand] = true;
    state->state->current_bet[state->state->current_hand] *= 2;

    draw(state);
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
            if (animated) {
                for (uint8_t i = 0; i < 4; i++) {
                    tweener_end(&(animator_tweener));
                }
                return;
            }
            if (selectedMenu == DoubleDown) {
                doubleDown(game_state);
            } else if (selectedMenu == Hit) {
                draw(game_state);
            } else if (selectedMenu == Stand) {
                animated = true;
                scheduler_start(&change_hand_schedule);
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
