#include "card.h"
#include "helpers.h"
#include "asset.h"

static Buffer *letters[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Buffer *suits[] = {NULL, NULL, NULL, NULL};

static Buffer *backSide = NULL;

void card_load_suit_assets(const Icon **icons) {
    for (uint8_t i = 0; i < 4; i++)
        suits[i] = asset_get_icon(icons[i]);
}

void card_load_letter_assets(const Icon **icons) {
    for (uint8_t i = 0; i < 13; i++)
        letters[i] = asset_get_icon(icons[i]);
}

void card_load_background(const Icon *icon) {
    backSide = asset_get_icon(icon);
}

void card_render_front(Card *c, int16_t x, int16_t y, bool selected, Buffer *buffer, uint8_t size_limit) {
    uint8_t height = fmin(size_limit, 11);

    buffer_draw_rbox(buffer, x - 9, y - 11, x + 8, y + height, White);
    buffer_draw_rbox_frame(buffer, x - 9, y - 11, x + 8, y + height, Black);


    buffer_set_sprite_rotation(0);

    Vector p = (Vector) {x - 3, y - 6};
    buffer_draw_all(buffer, letters[c->value], &p);

    p = (Vector) {(float) x + 3, (float) y - 6};
    buffer_draw_all(buffer, suits[c->suit], &p);

    buffer_set_sprite_rotation(180);


    if (size_limit > 8) {
        p = (Vector) {(float) x + 3, (float) y + 6};
        buffer_draw_all(buffer, letters[c->value], &p);
        p = (Vector) {(float) x - 4, (float) y + 6};
        buffer_draw_all(buffer, suits[c->suit], &p);
    }
    if (selected) {
        buffer_draw_box(buffer, x, y, x + 17, height + 1, Flip);
    }
}

void card_render_slot(int16_t x, int16_t y, bool selected, Buffer *buffer) {

    buffer_draw_rbox(buffer, x, y, x + 17, y + 23, Black);
    buffer_draw_rbox_frame(buffer, x + 2, y + 2, x + 14, y + 20, White);
    if (selected)
        buffer_draw_rbox(buffer, x + 1, y + 1, x + 16, y + 22, Flip);
}

void card_render_back(int16_t x, int16_t y, bool selected, Buffer *buffer, uint8_t size_limit) {
    uint8_t height = fmin(size_limit, 11);

    buffer_draw_rbox(buffer, x - 8, y - 11, x + 8, y + height, White);
    buffer_draw_rbox_frame(buffer, x - 8, y - 11, x + 8, y + height, Black);

    Vector pos = (Vector) {(float) x, (float) y};
    check_pointer(buffer);
    check_pointer(backSide);
    check_pointer(&pos);

    buffer_draw(buffer, backSide, &pos, 15, (int) fmin(size_limit, 22));
    if (selected) {
        buffer_draw_box(buffer, x, y, x + 17, height + 1, Flip);
    }
}

void card_try_render(Card *c, int16_t x, int16_t y, bool selected, Buffer *buffer, uint8_t size_limit) {
    if (c) {
        if (c->exposed)
            card_render_front(c, x, y, selected, buffer, size_limit);
        else
            card_render_back(x, y, selected, buffer, size_limit);
    } else {
        card_render_slot(x, y, selected, buffer);
    }
}

bool card_test_foundation(Card *data, Card *target) {
    if (!target || (target->value == -1 && target->suit == data->suit)) {
        return data->value == ACE;
    }
    return target->suit == data->suit && ((target->value + 1) % 13 == data->value % 13);
}

bool card_test_column(Card *data, Card *target) {
    if (!target) return data->value == KING;
    return target->suit % 2 == (data->suit + 1) % 2 && (data->value + 1) == target->value;
}

List *deck_generate(uint8_t deck_count) {
    List *deck = list_make();
    int cards_count = 52 * deck_count;
    uint8_t cards[cards_count];
    for (int i = 0; i < cards_count; i++) cards[i] = i % 52;
    srand(curr_time());

    //reorder
    for (int i = 0; i < cards_count; i++) {
        int r = i + (rand() % (cards_count - i));
        uint8_t card = cards[i];
        cards[i] = cards[r];
        cards[r] = card;
    }

    //Init deck list
    for (int i = 0; i < cards_count; i++) {
        Card *c = allocate(sizeof(Card));
        c->value = cards[i] % 13;
        c->suit = cards[i] / 13;
        c->exposed = false;
        list_push_back(c, deck);
    }

    return deck;
}

void deck_free(List *deck) {
    list_free(deck);
}

void deck_render_vertical(List *deck, uint8_t x, uint8_t y, int8_t selected, Buffer *buffer) {

    check_pointer(deck);
    check_pointer(buffer);
    uint8_t loop_end = deck->count;
    int8_t selection = loop_end - selected;
    uint8_t loop_start = MAX(loop_end - 4, 0);
    uint8_t position = 0;
    int8_t first_non_flipped;
    Card *first_non_flipped_card = deck_first_non_flipped(deck, &first_non_flipped);

    bool had_top = false;
    bool showDark = selection >= 0;

    if (first_non_flipped <= loop_start && selection != first_non_flipped && first_non_flipped_card) {
        // Draw a card back if it is not the first card
        if (first_non_flipped > 0) {
            card_render_back(x, y + position, false, buffer, 5);
            // Increment loop start index and position
            position += 4;
            loop_start++;
            had_top = true;
        }

        // Draw the front side of the first non-flipped card
        card_try_render(first_non_flipped_card, x, y + position, false, buffer, deck->count == 1 ? 22 : 9);

        position += 8;
        loop_start++; // Increment loop start index
    }

    // Draw the selected card with adjusted visibility
    if (loop_start > selection) {
        if (!had_top && first_non_flipped > 0) {
            card_render_back(x, y + position, false, buffer, 5);
            position += 4;
            loop_start++;
        }

        Card *selected_card = (Card *) list_peek_index(deck, selection);
        check_pointer(selected_card);
        // Draw the front side of the selected card
        card_try_render(selected_card, x, y + position, showDark, buffer, 9);
        position += 8;
        loop_start++; // Increment loop start index
    }

    int height = 5;
    ListItem *curr = list_get_index(deck, loop_start);
    for (uint8_t i = loop_start; i < loop_end; i++) {
        check_pointer(curr);
        if (!curr) break;

        if (i >= loop_start && i < loop_end) {
            height = 5;
            if ((i + 1) == loop_end) height = 22;
            else if (i == selection || i == first_non_flipped) height = 9;
            Card *c = (Card *) curr->data;
            check_pointer(c);
            card_try_render(c, x, y + position, i == selection && showDark, buffer, height);
            if (i == selection || i == first_non_flipped)position += 4;
            position += 4;
        }
        curr = curr->next;
    }
}

void render_pile(List *deck, bool flipped, int16_t x, int16_t y, Buffer *buffer) {
    //6 in row
    ListItem *c = deck->head;//flipped ? deck->tail : deck->head;
    int8_t mod = 6;
    int8_t startX = flipped ? (deck->count % mod) * 8 : 0;

    if (flipped) {
        if (((uint8_t) deck->count) >= mod)
            startX = mod * 8;
        startX -= 8;
    }

    int8_t startY = flipped ? (int8_t) ((deck->count - 1) / mod) * 9 : 0;
    uint8_t id = 0, id2 = 0;

    while (c) {
        id2 = (int8_t) id / mod;
        uint8_t cx = x + (id % mod) * 8 + (id2 * 4) - startX;
        uint8_t cy = y + id2 * 9 - startY;
        card_render_front(c->data, cx, cy, false, buffer, 22);

        id++;
//        c=flipped? c->prev : c->next;
        c = c->next;
    }
}

void deck_render(List *deck, DeckType type, int16_t x, int16_t y, int8_t selected, bool draw_empty, Buffer *buffer) {
    switch (type) {
        case Normal:
            card_try_render(list_peek_back(deck), x, y, selected == 1, buffer, 22);
            break;
        case Vertical:
            if (deck && deck->count > 0)
                deck_render_vertical(deck, x, y, selected, buffer);
            else if (draw_empty)
                card_render_slot(x, y, selected == 1, buffer);
            break;
        case Pile:
        case PileUpsideDown:    //same as pile, but grows from bottom-right to top-left
            render_pile(deck, type == PileUpsideDown, x, y, buffer);
            break;
        case Covered:
            if (deck->count > 0) {
                card_render_back(x, y, selected == 1, buffer, 22);
                if (deck->count > 1) {
                    Card *c = list_peek_index(deck, 1);
                    if (c) {
                        card_render_front(c, x + 10, y + 5, selected == 1, buffer, 22);
                    }
                }
            } else if (draw_empty)
                card_render_slot(x, y, selected == 1, buffer);
            break;
    }
}

Card *deck_first_non_flipped(List *deck, int8_t *index) {
    ListItem *curr = deck->head;
    for (int8_t i = 0; i < (int8_t) deck->count; i++) {
        if (!curr->data) break;
        Card *card = (Card *) curr->data;
        if (card->exposed) {
            (*index) = i;
            return card;
        }
        curr = curr->next;
    }
    (*index) = -1;
    return NULL;
}

uint8_t hand_value(List *deck, uint8_t max) {
    uint8_t value = 0;
    ListItem *curr = deck->head;
    uint8_t ace_count = 0;
    for (int8_t i = 0; i < (int8_t) deck->count; i++) {
        if (!curr->data) break;
        Card *card = (Card *) curr->data;
        if (card->value == ACE) ace_count++;
        else value += MIN(card->value + 2, 10);

        curr = curr->next;
    }

    while (ace_count > 0) {
        if ((value + (11 * ace_count)) <= max) {
            value += 11;
        } else {
            value++;
        }
        ace_count--;
    }

    return value;
}

static Transform transform = {IDENTITY_MATRIX, IDENTITY_MATRIX, IDENTITY_MATRIX, IDENTITY_MATRIX};

void card_compute_animation_state(CardAnimatorData *data, float delta, float speed) {
    if (data->finished) return;

    if (data->state >= 1) {
        data->finished = true;
        return;
    }

    data->state += delta * speed;
    if (data->state > 1)data->state = 1;
    float rotation = lerp_number(data->start_rotation, data->end_rotation, data->state);
    Vector scale;
    vector_lerp(&(data->start_scale), &(data->end_scale), data->state, &scale);
    Vector position;
    vector_lerp(&(data->start_position), &(data->end_position), data->state, &position);

    compute_transformation_matrix(&position, &scale, rotation, &transform);
    matrix_copy(&(transform.transformation_matrix), &(data->transformMatrix));
}
