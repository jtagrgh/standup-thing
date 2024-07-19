#ifndef ROTATE_VECTOR_H
#define ROTATE_VECTOR_H

#include "vec3d.h"

typedef enum RotateVectorStatus {
    ROTATE_VECTOR_OK,
    ROTATE_VECTOR_NOTOK
} RotateVectorStatus;

typedef struct RotateVectorState {
    Vec3d value;
} RotateVectorState;

void init_rotation_vector_state(RotateVectorState *state);

RotateVectorStatus rotate_vector(RotateVectorState *state, Vec3d a, Vec3d rotation_vector);

#endif