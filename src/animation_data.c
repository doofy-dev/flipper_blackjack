#include "animation_data.h"
#include "graphics/buffer.h"

static const PackedTransform DECK_START = {
    .position = {64,6},
    .scale = {1,-1},
    .rotation = 0
};

const AnimationData INTRO_DEALING[4] = {
    {
        // Dealer first card
        .start={.position = DECK_START.position, .scale = {-1,-1}, .rotation = DECK_START.rotation},
        // flip is based on y sign, keep negative
        .end = {.position = {10,13}, .scale = {-1,-1}, .rotation=180},
        .duration=0.75f
    },
    // Player first
    {
        .start=DECK_START,
        .end={.position = {SCREEN_WIDTH-18, 50}, .scale = {1,1}, .rotation = 0},
        .duration=1
    },
    // Dealer second
   {
        .start=DECK_START,
        .end={.position = {20,18}, .scale = {1,1}, .rotation = 0},
        .duration=0.75f
    },
    // Player second
    {
        .start=DECK_START,
        .end={.position = {SCREEN_WIDTH-10, 50}, .scale = {1,1}, .rotation = 0},
        .duration=1
    },
};