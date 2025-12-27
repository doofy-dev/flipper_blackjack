#pragma once

#include <furi.h>
#include "scene.h"
#include "../math/equation.h"
#include "../math/matrix.h"

typedef struct Tweener Tweener;
typedef struct AnimationData AnimationData;

struct Tweener {
    const AnimationData *animation_data;
    PackedTransform current_transform;
    Matrix transform_matrix;
    void *data;
    float delay;
    float t;
    bool finished;
    bool (*update)(Tweener *);
};

void tweener_start(Tweener *tweener);
void tweener_end(Tweener *tweener);

void tweener_update();

void tweener_prepare(SceneData *data);

void tweener_cleanup();
