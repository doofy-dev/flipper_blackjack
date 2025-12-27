#pragma once

#include "vector.h"
typedef float Matrix[9];
typedef struct Transform Transform;
typedef union  PackedTransform PackedTransform;

#define IDENTITY_MATRIX {1,0,0, 0,1,0, 0,0,1}

#define MAKE_TRANSFORM() (Transform){ \
    .dirty=false, \
    .scale={1,1}, \
    .position={0,0}, \
    .rotation=0, \
    .transformation_matrix=IDENTITY_MATRIX \
}

struct Transform {
    bool dirty;
    Vector scale;
    Vector position;
    float rotation;

    Matrix transformation_matrix;
};


void matrix_scale(Vector *scale, Matrix *target);

void matrix_reset(Matrix *target);

void matrix_translate(Vector *translate, Matrix *target);

void matrix_copy(Matrix *source, Matrix *target);

void matrix_rotate(float rotation, Matrix *target);

void matrix_mul(Matrix *a, Matrix *b, Matrix *target);

void matrix_mul_vector(Matrix *a, Vector *vector, Vector *target);

void matrix_get_translation(Matrix *a, Vector *translation);

float matrix_get_rotation(Matrix *a);

void matrix_get_scaling(Matrix *a, Vector *target);

void matrix_forward(Matrix *a, Vector *target);

void compute_transformation_matrix(Transform *target, Transform *parent);
void compute_transformation_matrix_packed(PackedTransform *current, Matrix *target);

void matrix_print(Matrix *a);
