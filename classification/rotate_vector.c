#include "rotate_vector.h"
#include <stdio.h>


void init_rotation_vector_state(RotateVectorState *state, uint8_t debug) {
    state->base.debug = debug;
    state->value = (Vec3d) {0, 0, 0};
}

RotateVectorStatus rotate_vector(RotateVectorState *state, Vec3d a, Vec3d rotation_vector) {
    state->value = vec3d_rotate_by_rotation_vector_assuming_things_that_arent_true(a, rotation_vector);

    if (state->base.debug) {
        printf("%f, %f, %f, ", state->value.x, state->value.y, state->value.z);
    }
    
    return ROTATE_VECTOR_OK;
}