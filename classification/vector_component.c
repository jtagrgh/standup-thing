#include "vector_component.h"
#include "vec3d.h"


void init_vector_component_state(VectorComponentState *state) {
    state->value = 0;
}

VectorComponentStatus vector_component(VectorComponentState *state, Vec3d a, Vec3d b) {
    state->value = vec3d_component(a, b);

    return VECTOR_COMPONENT_OK;
}