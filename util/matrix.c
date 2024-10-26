#include "matrix.h"
#include "helpers.h"

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

void matrix_reset(Matrix *target){
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

void matrix_get_scaling(Matrix *data, Vector *target){
    target->x = sqrtf((*data)[0] * (*data)[0] + (*data)[1] * (*data)[1]);
    target->y = sqrtf((*data)[3] * (*data)[3] + (*data)[4] * (*data)[4]);
}

void compute_transformation_matrix(Vector *pos, Vector* scale, float rotation, Transform* target){
    matrix_scale(scale, &(target->scale_matrix));
    matrix_translate(pos, &(target->position_matrix));

    matrix_rotate(rotation * DEG_2_RAD, &(target->rotation_matrix));
    Matrix temp = IDENTITY_MATRIX;

    matrix_mul(&(target->position_matrix), &(target->rotation_matrix), &temp);

    matrix_mul(&temp, &(target->scale_matrix), &(target->transformation_matrix));
}

void matrix_copy(Matrix *source, Matrix *target){
    memcpy(target, source, sizeof(Matrix));
}