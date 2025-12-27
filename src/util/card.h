#pragma once

#include <furi.h>
#include "../graphics/render.h"
#include "../graphics/buffer.h"
#include "../math/matrix.h"
#include "list.h"
#include "tweener.h"

typedef enum {
    NONE = -1,
    TWO = 0,        //1
    THREE = 1,      //2
    FOUR = 2,       //3
    FIVE = 3,       //4
    SIX = 4,        //5
    SEVEN = 5,      //6
    EIGHT = 6,      //7
    NINE = 7,       //8
    TEN = 8,        //9
    JACK = 9,       //10
    QUEEN = 10,     //11
    KING = 11,      //12
    ACE = 12,       //13
} CardValue;

typedef struct {
    uint8_t suit;
    CardValue value;
    bool exposed;
} Card;

typedef enum {
    Normal,
    Vertical,
    Pile,
    PileUpsideDown,
    Covered
} DeckType;

typedef struct {
    Vector start_position;
    Vector end_position;

    float start_rotation;
    float end_rotation;

    Vector start_scale;
    Vector end_scale;

    Matrix transformMatrix;
    Card *card;
    bool flip;
} CardAnimatorData;

uint8_t hand_value(List *deck, uint8_t max);

void card_set_transform_parent(Matrix *parent);

void card_render_front(Card *c, int16_t x, int16_t y, bool selected, Buffer *buffer);

void card_render_slot(int16_t x, int16_t y, bool selected, Buffer *buffer);

void card_render_back(int16_t x, int16_t y, bool selected, Buffer *buffer);

void card_try_render(Card *c, int16_t x, int16_t y, bool selected, Buffer *buffer);

bool card_test_foundation(Card *data, Card *target);

bool card_test_column(Card *data, Card *target);

List *deck_generate(uint8_t deck_count);

void deck_free(List *deck);

Card* deck_first_non_flipped(List *deck, int8_t *index);

void deck_render(List *deck, DeckType type, int16_t x, int16_t y, int8_t selected, bool draw_empty, Buffer *buffer);

void set_card_flip_uv(FlipMode mode);

void card_load_suit_assets(const Icon **icons);
void card_load_letter_assets(const Icon **icons);
void card_load_background(const Icon *icon);
bool card_compute_animation_state(Tweener *data);
// void card_draw_tweener(Tweener *tweener,Buffer *buffer);
