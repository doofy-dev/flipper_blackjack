#include "card.h"
#include "helpers.h"
#include "../math/equation.h"
#include "../graphics/render.h"
#include "../graphics/asset.h"

static Buffer* letters[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static Buffer* suits[] = {NULL, NULL, NULL, NULL};

static Buffer* backSide = NULL;
static Transform* parent_transform = NULL;
static FlipMode flip_mode = FLIP_NONE;

void set_card_flip_uv(FlipMode mode)
{
    flip_mode = mode;
}

void card_set_transform_parent(Matrix* parent)
{
    if (parent)
    {
        parent_transform = allocate(sizeof(Transform));
        matrix_copy(parent, &(parent_transform->transformation_matrix));
    }
    else
    {
        release(parent_transform);
    }
}

void card_load_suit_assets(const Icon** icons)
{
    for (uint8_t i = 0; i < 4; i++)
        suits[i] = asset_get_icon(icons[i]);
}

void card_load_letter_assets(const Icon** icons)
{
    for (uint8_t i = 0; i < 13; i++)
        letters[i] = asset_get_icon(icons[i]);
}

void card_load_background(const Icon* icon)
{
    backSide = asset_get_icon(icon);
}

void card_render_front(Card* c, int16_t x, int16_t y, bool selected, Buffer* buffer)
{
    Transform card_transform = {
        .scale = {1, 1},
        .dirty = true,
        .position = {x, y},
        .rotation = 0,
        .transformation_matrix = IDENTITY_MATRIX
    };
    RenderData card_data = {
        .poly = RECTANGLE(-8, -11, 17, 23),
        .color = COLOR_BLACK,
        .mask_color = selected ? COLOR_BLACK : COLOR_WHITE,
        .callback = render_rbox
    };
    flip_uv(&card_data.poly, flip_mode);


    compute_transformation_matrix(&card_transform, parent_transform);
    cache_position(&card_data.poly, &card_transform);
    rasterize(buffer, &card_data);


    Transform quadrant_transform = {
        .scale = {1, 1},
        .dirty = true,
        .position = {0, -9},
        .rotation = 0,
        .transformation_matrix = IDENTITY_MATRIX
    };

    //letter 5x5, suit 7x7
    RenderData quadrant_data = {
        .sprite = suits[c->suit],
        .tile_mode = TILE_NONE,
        .poly = RECTANGLE(0, 0, 7, 7),
        .color = selected ? COLOR_WHITE : COLOR_BLACK
    };
    flip_uv(&quadrant_data.poly, flip_mode);

    compute_transformation_matrix(&quadrant_transform, &card_transform);
    cache_position(&quadrant_data.poly, &quadrant_transform);
    rasterize(buffer, &quadrant_data);

    quadrant_transform.dirty = true;
    quadrant_transform.position.x = 1;
    quadrant_transform.position.y = 10;
    quadrant_transform.rotation = 180;

    compute_transformation_matrix(&quadrant_transform, &card_transform);
    cache_position(&quadrant_data.poly, &quadrant_transform);
    rasterize(buffer, &quadrant_data);


    quadrant_data.poly = RECTANGLE(0, 0, 5, 5);
    flip_uv(&quadrant_data.poly, flip_mode);
    quadrant_data.sprite = letters[c->value];
    quadrant_transform.dirty = true;
    quadrant_transform.position.x = -6;
    quadrant_transform.position.y = -8;
    quadrant_transform.rotation = 0;

    compute_transformation_matrix(&quadrant_transform, &card_transform);
    cache_position(&quadrant_data.poly, &quadrant_transform);
    rasterize(buffer, &quadrant_data);

    quadrant_transform.dirty = true;
    quadrant_transform.position.x = 7;
    quadrant_transform.position.y = 9;
    quadrant_transform.rotation = 180;

    compute_transformation_matrix(&quadrant_transform, &card_transform);
    cache_position(&quadrant_data.poly, &quadrant_transform);
    rasterize(buffer, &quadrant_data);
}

void card_render_slot(int16_t x, int16_t y, bool selected, Buffer* buffer)
{
    Transform card_transform = {
        .scale = {1, 1},
        .dirty = true,
        .position = {x, y},
        .rotation = 0,
        .transformation_matrix = IDENTITY_MATRIX
    };
    RenderData card_data = {
        .poly = RECTANGLE(-8, -11, 17, 23),
        .color = COLOR_BLACK,
        .mask_color = selected ? COLOR_WHITE : COLOR_BLACK,
        .callback = render_rbox
    };

    compute_transformation_matrix(&card_transform, parent_transform);
    cache_position(&card_data.poly, &card_transform);
    rasterize(buffer, &card_data);

    card_data.poly = RECTANGLE(-5, -8, 11, 17);
    card_data.mask_color = COLOR_TRANSPARENT;
    card_data.color = COLOR_FLIP;
    card_transform.dirty = true;
    cache_position(&card_data.poly, &card_transform);
    rasterize(buffer, &card_data);
}

Poly cardPoly = RECTANGLE(-8, -11, 16, 22);

void card_render_back(int16_t x, int16_t y, bool selected, Buffer* buffer)
{
    Transform card_transform = {
        .scale = {1, 1},
        .dirty = true,
        .position = {x, y},
        .rotation = 0,
        .transformation_matrix = IDENTITY_MATRIX
    };
    RenderData card_data = {
        .poly = RECTANGLE(-8, -11, 17, 23),
        .color = COLOR_BLACK,
        .mask_color = selected ? COLOR_BLACK : COLOR_WHITE,
        .callback = render_rbox
    };
    flip_uv(&card_data.poly, flip_mode);

    compute_transformation_matrix(&card_transform, parent_transform);
    cache_position(&card_data.poly, &card_transform);
    rasterize(buffer, &card_data);

    card_data.poly = RECTANGLE(-7, -10, 15, 21);
    flip_uv(&card_data.poly, flip_mode);
    card_data.sprite = backSide;
    card_data.color = selected ? COLOR_WHITE : COLOR_BLACK;
    card_data.callback = NULL;
    card_transform.dirty = true;
    cache_position(&card_data.poly, &card_transform);
    rasterize(buffer, &card_data);
}

void card_try_render(Card* c, int16_t x, int16_t y, bool selected, Buffer* buffer)
{
    if (c)
    {
        if (c->exposed)
            card_render_front(c, x, y, selected, buffer);
        else
            card_render_back(x, y, selected, buffer);
    }
    else
    {
        card_render_slot(x, y, selected, buffer);
    }
}

bool card_test_foundation(Card* data, Card* target)
{
    if (!target || (target->value == -1 && target->suit == data->suit))
    {
        return data->value == ACE;
    }
    return target->suit == data->suit && ((target->value + 1) % 13 == data->value % 13);
}

bool card_test_column(Card* data, Card* target)
{
    if (!target) return data->value == KING;
    return target->suit % 2 == (data->suit + 1) % 2 && (data->value + 1) == target->value;
}

List* deck_generate(uint8_t deck_count)
{
    List* deck = list_make();
    int cards_count = 52 * deck_count;
    uint8_t cards[cards_count];
    for (int i = 0; i < cards_count; i++) cards[i] = i % 52;
    srand(curr_time());

    //reorder
    for (int i = 0; i < cards_count; i++)
    {
        int r = i + (rand() % (cards_count - i));
        uint8_t card = cards[i];
        cards[i] = cards[r];
        cards[r] = card;
    }

    //Init deck list
    for (int i = 0; i < cards_count; i++)
    {
        Card* c = allocate(sizeof(Card));
        c->value = cards[i] % 13;
        c->suit = cards[i] / 13;
        c->exposed = false;
        list_push_back(c, deck);
    }

    return deck;
}

void deck_free(List* deck)
{
    list_free(deck);
}

void deck_render_vertical(List* deck, uint8_t x, uint8_t y, int8_t selected, Buffer* buffer)
{
    check_pointer(deck);
    check_pointer(buffer);
    uint8_t loop_end = deck->count;
    int8_t selection = loop_end - selected;
    uint8_t loop_start = MAX(loop_end - 4, 0);
    uint8_t position = 0;
    int8_t first_non_flipped;
    Card* first_non_flipped_card = deck_first_non_flipped(deck, &first_non_flipped);

    bool had_top = false;
    bool showDark = selection >= 0;

    if (first_non_flipped <= loop_start && selection != first_non_flipped && first_non_flipped_card)
    {
        // Draw a card back if it is not the first card
        if (first_non_flipped > 0)
        {
            card_render_back(x, y + position, false, buffer);
            // Increment loop start index and position
            position += 4;
            loop_start++;
            had_top = true;
        }

        // Draw the front side of the first non-flipped card
        card_try_render(first_non_flipped_card, x, y + position, false, buffer);

        position += 8;
        loop_start++; // Increment loop start index
    }

    // Draw the selected card with adjusted visibility
    if (loop_start > selection)
    {
        if (!had_top && first_non_flipped > 0)
        {
            card_render_back(x, y + position, false, buffer);
            position += 4;
            loop_start++;
        }

        Card* selected_card = (Card*)list_peek_index(deck, selection);
        check_pointer(selected_card);
        // Draw the front side of the selected card
        card_try_render(selected_card, x, y + position, showDark, buffer);
        position += 8;
        loop_start++; // Increment loop start index
    }

    ListItem* curr = list_get_index(deck, loop_start);
    for (uint8_t i = loop_start; i < loop_end; i++)
    {
        check_pointer(curr);
        if (!curr) break;

        if (i >= loop_start && i < loop_end)
        {
            Card* c = (Card*)curr->data;
            check_pointer(c);
            card_try_render(c, x, y + position, i == selection && showDark, buffer);
            if (i == selection || i == first_non_flipped)position += 4;
            position += 4;
        }
        curr = curr->next;
    }
}

void render_pile(List* deck, bool flipped, int16_t x, int16_t y, Buffer* buffer)
{
    int8_t mod = 6; // Number of cards in a row
    int8_t startX = flipped ? (deck->count % mod) * 8 : 0;

    if (flipped)
    {
        if (((uint8_t)deck->count) >= mod)
            startX = mod * 8;
        startX -= 8;
    }

    int8_t startY = flipped ? (int8_t)((deck->count - 1) / mod) * 9 : 0;
    uint8_t id = 0, id2 = 0;

    FOREACH(c, deck)
    {
        id2 = (int8_t)id / mod;
        uint8_t cx = x + (id % mod) * 8 + (id2 * 4) - startX;
        uint8_t cy = y + id2 * 9 - startY;
        card_render_front(c->data, cx, cy, false, buffer);

        id++;
    }
}

void deck_render(List* deck, DeckType type, int16_t x, int16_t y, int8_t selected, bool draw_empty, Buffer* buffer)
{
    switch (type)
    {
    case Normal:
        card_try_render(list_peek_back(deck), x, y, selected == 1, buffer);
        break;
    case Vertical:
        if (deck && deck->count > 0)
            deck_render_vertical(deck, x, y, selected, buffer);
        else if (draw_empty)
            card_render_slot(x, y, selected == 1, buffer);
        break;
    case Pile:
    case PileUpsideDown: //same as pile, but grows from bottom-right to top-left
        render_pile(deck, type == PileUpsideDown, x, y, buffer);
        break;
    case Covered:
        if (deck->count > 0)
        {
            card_render_back(x, y, selected == 1, buffer);
            if (deck->count > 1)
            {
                Card* c = list_peek_index(deck, 1);
                if (c)
                {
                    card_render_front(c, x + 10, y + 5, selected == 1, buffer);
                }
            }
        }
        else if (draw_empty)
            card_render_slot(x, y, selected == 1, buffer);
        break;
    }
}

Card* deck_first_non_flipped(List* deck, int8_t* index)
{
    ListItem* curr = deck->head;
    for (int8_t i = 0; i < (int8_t)deck->count; i++)
    {
        if (!curr->data) break;
        Card* card = (Card*)curr->data;
        if (card->exposed)
        {
            (*index) = i;
            return card;
        }
        curr = curr->next;
    }
    (*index) = -1;
    return NULL;
}

uint8_t hand_value(List* deck, uint8_t max)
{
    uint8_t value = 0;
    ListItem* curr = deck->head;
    uint8_t ace_count = 0;
    for (int8_t i = 0; i < (int8_t)deck->count; i++)
    {
        if (!curr->data) break;
        Card* card = (Card*)curr->data;
        if (card->value == ACE) ace_count++;
        else value += MIN(card->value + 2, 10);

        curr = curr->next;
    }

    while (ace_count > 0)
    {
        if ((value + (11 * ace_count)) <= max)
        {
            value += 11;
        }
        else
        {
            value++;
        }
        ace_count--;
    }

    return value;
}

bool card_compute_animation_state(Tweener* tweener)
{
    Card* card = tweener->data;
    card->exposed = tweener->current_transform.scale.y > 0;
    return false;
}

/*

void card_draw_tweener(Tweener* tweener, Buffer* buffer)
{
    UNUSED(tweener);
    UNUSED(buffer);
   CardAnimatorData *animatorData = tweener->data;
    buffer_set_transform(&(animatorData->transformMatrix));
    card_try_render(animatorData->card, 0, 0, false, buffer, 22);#1#
}
*/
