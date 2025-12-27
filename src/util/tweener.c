#include "tweener.h"
#include "helpers.h"
#include "../animation_data.h"

static List *tweeners = NULL;
static SceneData *tweener_scene_data;

void tweener_prepare(SceneData *data) {
    if (!tweeners)
        tweeners = list_make();
    tweener_scene_data = data;
}

void tweener_cleanup() {
    list_free(tweeners);
}

void tweener_start(Tweener *tweener) {
    tweener->t = 0;
    matrix_reset(&tweener->transform_matrix);
    tweener->current_transform = tweener->animation_data->start;
    compute_transformation_matrix_packed(&tweener->current_transform, &tweener->transform_matrix);

    tweener->finished = false;
    tweener->finished = tweener->update(tweener);

    list_push_back(tweener, tweeners);
}

void tweener_update() {
    if (!tweeners) return;

    FOREACH (tween, tweeners) {
        tweener_scene_data->dirty=true;
        Tweener *data = tween->data;
        if (data) {
            if (data->delay <= 0) {
                data->t += tweener_scene_data->delta_time * (1.f / data->animation_data->duration);
                if (data->t > 1) data->t = 1;
                lerp_transform(&(data->animation_data->start), &(data->animation_data->end), data->t, &data->current_transform);
                compute_transformation_matrix_packed(&data->current_transform, &data->transform_matrix);

                data->finished = data->update(data) || data->t == 1;
                if (data->finished) {
                    list_remove_item(data, tweeners);
                }
            } else {
                data->delay -= tweener_scene_data->delta_time;
            }
        }
    }
}

void tweener_end(Tweener *tweener) {
    check_pointer(tweener);
    tweener->finished=true;
    tweener->t=1;
    list_remove_item(tweener, tweeners);
}