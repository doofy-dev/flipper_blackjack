#pragma once

#include "vector.h"
typedef float Matrix[9];

typedef struct {
    Matrix position_matrix;
    Matrix scale_matrix;
    Matrix rotation_matrix;

    Matrix transformation_matrix;
} Transform;


#define IDENTITY_MATRIX {1,0,0, 0,1,0, 0,0,1}

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

void compute_transformation_matrix(Vector *pos, Vector* scale, float rotation, Transform* target);