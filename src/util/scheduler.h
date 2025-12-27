#pragma once


#include "scene.h"

typedef struct{
    void (*callback)(void *data, SceneData *sceneData);
    float time_left;
    float timeout;
    bool repeat;
    void *data;
} Scheduler;

void scheduler_prepare(SceneData *sceneData);
void scheduler_cleanup();
void scheduler_start(Scheduler *scheduler);
void scheduler_update();
void scheduler_stop(Scheduler *scheduler);
void scheduler_stop_all();