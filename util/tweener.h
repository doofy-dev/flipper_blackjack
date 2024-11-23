#pragma once

#include <furi.h>
#include "scene.h"

typedef struct Tweener Tweener;

struct Tweener {
    void *data;
    float length;
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
