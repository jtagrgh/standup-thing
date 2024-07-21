#include "vector_component.h"
#include "../commons/vec3d.h"
#include <stdio.h>


void init_vector_component_state(VectorComponentState *state) {
    state->value = 0;
}

VectorComponentStatus vector_component(VectorComponentState *state, Vec3d a, Vec3d b) {
    Vec3d a_less_b = vec3d_sub_vectors(a, b);
    state->value = vec3d_component(a_less_b, b);

    if (state->base.debug) {
        printf("%s:%f,", state->base.name, state->value);
    }

    return VECTOR_COMPONENT_OK;
}