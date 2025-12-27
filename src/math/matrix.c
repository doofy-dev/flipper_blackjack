#include "matrix.h"
#include "../util/helpers.h"
#include "equation.h"

void matrix_scale(Vector *scale, Matrix *target) {
    (*target)[0] = scale->x;
    (*target)[1] = 0;
    (*target)[2] = 0;

    (*target)[3] = 0;
    (*target)[4] = scale->y;
    (*target)[5] = 0;

    (*target)[6] = 0;
    (*target)[7] = 0;
    (*target)[8] = 1;
}

void matrix_reset(Matrix *target) {
    (*target)[0] = 1;
    (*target)[1] = 0;
    (*target)[2] = 0;

    (*target)[3] = 0;
    (*target)[4] = 1;
    (*target)[5] = 0;

    (*target)[6] = 0;
    (*target)[7] = 0;
    (*target)[8] = 1;
}

void matrix_translate(Vector *translate, Matrix *target) {
    (*target)[0] = 1;
    (*target)[1] = 0;
    (*target)[2] = translate->x;

    (*target)[3] = 0;
    (*target)[4] = 1;
    (*target)[5] = translate->y;

    (*target)[6] = 0;
    (*target)[7] = 0;
    (*target)[8] = 1;
}

void matrix_rotate(float angle, Matrix *target) {
    (*target)[0] = cosf(angle);
    (*target)[1] = -sinf(angle);
    (*target)[2] = 0;

    (*target)[3] = sinf(angle);
    (*target)[4] = cosf(angle);
    (*target)[5] = 0;

    (*target)[6] = 0;
    (*target)[7] = 0;
    (*target)[8] = 1;
}

void matrix_mul(Matrix *a, Matrix *b, Matrix *target) {
    (*target)[0] = (*a)[0] * (*b)[0] + (*a)[1] * (*b)[3] + (*a)[2] * (*b)[6];
    (*target)[1] = (*a)[0] * (*b)[1] + (*a)[1] * (*b)[4] + (*a)[2] * (*b)[7];
    (*target)[2] = (*a)[0] * (*b)[2] + (*a)[1] * (*b)[5] + (*a)[2] * (*b)[8];

    (*target)[3] = (*a)[3] * (*b)[0] + (*a)[4] * (*b)[3] + (*a)[5] * (*b)[6];
    (*target)[4] = (*a)[3] * (*b)[1] + (*a)[4] * (*b)[4] + (*a)[5] * (*b)[7];
    (*target)[5] = (*a)[3] * (*b)[2] + (*a)[4] * (*b)[5] + (*a)[5] * (*b)[8];

    (*target)[6] = (*a)[6] * (*b)[0] + (*a)[7] * (*b)[3] + (*a)[8] * (*b)[6];
    (*target)[7] = (*a)[6] * (*b)[1] + (*a)[7] * (*b)[4] + (*a)[8] * (*b)[7];
    (*target)[8] = (*a)[6] * (*b)[2] + (*a)[7] * (*b)[5] + (*a)[8] * (*b)[8];
}

void matrix_mul_vector(Matrix *a, Vector *vector, Vector *target) {
    target->x = (*a)[0] * vector->x + (*a)[1] * vector->y + (*a)[2];
    target->y = (*a)[3] * vector->x + (*a)[4] * vector->y + (*a)[5];
}

void matrix_get_translation(Matrix *a, Vector *translation) {
    translation->x = (*a)[2];
    translation->y = (*a)[5];
}

float matrix_get_rotation(Matrix *a) {
    return atan2f((*a)[3], (*a)[4]);
}

void matrix_get_scaling(Matrix *data, Vector *target) {
    target->x = sqrtf((*data)[0] * (*data)[0] + (*data)[1] * (*data)[1]);
    target->y = sqrtf((*data)[3] * (*data)[3] + (*data)[4] * (*data)[4]);
}

static Matrix scale_matrix, rotation_matrix, translation_matrix;

void compute_transformation_matrix(Transform *target, Transform *parent) {
    if (target->dirty == false) return;

    // Apply scaling
    matrix_scale(&(target->scale), &scale_matrix);

    // Apply rotation
    matrix_rotate(target->rotation * DEG_2_RAD, &rotation_matrix);

    // Apply translation
    matrix_translate(&(target->position), &translation_matrix);

    // Correct order: First scale, then rotate, finally translate
    // Combine transformations
    Matrix temp_matrix;
    matrix_mul(&translation_matrix, &rotation_matrix, &temp_matrix); // Rotate and then scale
    matrix_mul(&temp_matrix, &scale_matrix, &(target->transformation_matrix)); // Apply translation after

    //Apply relative transform from parent node
    if (parent) {
        matrix_mul(&(parent->transformation_matrix), &(target->transformation_matrix), &temp_matrix);
        matrix_copy(&temp_matrix, &(target->transformation_matrix));
    }
}

void compute_transformation_matrix_packed(PackedTransform *current, Matrix *target)
{
    // Apply scaling
    matrix_scale(&(current->scale), &scale_matrix);

    // Apply rotation
    matrix_rotate(current->rotation * DEG_2_RAD, &rotation_matrix);

    // Apply translation
    matrix_translate(&(current->position), &translation_matrix);

    // Correct order: First scale, then rotate, finally translate
    // Combine transformations
    Matrix temp_matrix;
    matrix_mul(&translation_matrix, &rotation_matrix, &temp_matrix); // Rotate and then scale
    matrix_mul(&temp_matrix, &scale_matrix, target); // Apply translation after
}

void matrix_copy(Matrix *source, Matrix *target) {
    memcpy(target, source, sizeof(Matrix));
}

void matrix_print(Matrix *a) {
    for (uint8_t i = 0; i < 3; i++)
        FURI_LOG_D("MAT", "%f %f %f",
               (double)(*a)[i*3],
               (double)(*a)[i*3+1],
               (double)(*a)[i*3+2]
    );
}

void matrix_forward(Matrix *a, Vector *target) {
    target->x = (*a)[0];
    target->y = (*a)[3];
}