#pragma once
#include "math/equation.h"

typedef struct AnimationData AnimationData;

struct AnimationData {
    PackedTransform start;
    PackedTransform end;
    float duration;
};

extern const AnimationData INTRO_DEALING[4];
