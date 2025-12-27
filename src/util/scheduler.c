#include "scheduler.h"
#include "list.h"

static List *schedules = NULL;
static SceneData *scheduler_scene_data;

void scheduler_prepare(SceneData *sceneData) {
    if (!schedules) {
        schedules = list_make();
    }
    scheduler_scene_data = sceneData;
}

void scheduler_cleanup() {
    list_free(schedules);
}

void scheduler_start(Scheduler *scheduler) {
    scheduler->time_left = scheduler->timeout;
    list_push_back(scheduler, schedules);
}

void scheduler_update() {
    if (!schedules) return;

    FOREACH(item, schedules) {
        Scheduler *current_scheduler = (Scheduler *) item->data;

        current_scheduler->time_left -= scheduler_scene_data->delta_time;

        if (current_scheduler->time_left <= 0) {
            current_scheduler->time_left = 0;
            current_scheduler->callback(current_scheduler->data, scheduler_scene_data);
            if (current_scheduler->repeat) {
                current_scheduler->time_left = current_scheduler->timeout;
            } else {
                list_remove_item(current_scheduler, schedules);
            }
        }

    }
}

void scheduler_stop(Scheduler *scheduler) {
    if (!schedules) return;

    FOREACH(schedule, schedules) {
        Scheduler *current_scheduler = (Scheduler *) schedule->data;
        if (scheduler == current_scheduler) {
            list_remove_item(current_scheduler, schedules);
        }

    }
}

void scheduler_stop_all() {
    list_clear(schedules);
}