#include "rotate_vector.h"

void init_rotation_vector_state(RotateVectorState *state) {
    state->value = (Vec3d) {0, 0, 0};
}

RotateVectorStatus rotate_vector(RotateVectorState *state, Vec3d a, Vec3d rotation_vector) {
    //state->value = vec3d_rotate_by_rotation_vector_assuming_things_that_arent_true(a, rotation_vector);
    state->value = vec3d_rotate_by_quaternion(a, rotation_vector, 1);
    return ROTATE_VECTOR_OK;
}