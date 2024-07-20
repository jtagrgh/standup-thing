#include "integral_3d.h"
#include "../commons/vec3d.h"
#include <stdio.h>


void init_integral_3d_state(Integral3dState *state, double interval, uint8_t debug) {
    state->base.debug = debug;
    state->value = (Vec3d){0, 0, 0};
    init_integral_state(&state->x, interval, debug);
    init_integral_state(&state->y, interval, debug);
    init_integral_state(&state->z, interval, debug);
}

void free_integral_3d_state(Integral3dState *state) {
    free_integral_state(&state->x);
    free_integral_state(&state->y);
    free_integral_state(&state->z);
}

Integral3dStatus integral_3d(Integral3dState *state, Vec3d new_sample) {
    IntegralStatus status;
    
    status = integral(&state->x, new_sample.x);
    if (status == INTEGRAL_NOTOK) {
        return INTEGRAL_3D_NOTOK;
    }
    status = integral(&state->y, new_sample.y);
    if (status == INTEGRAL_NOTOK) {
        return INTEGRAL_3D_NOTOK;
    }
    status = integral(&state->z, new_sample.z);
    if (status == INTEGRAL_NOTOK) {
        return INTEGRAL_3D_NOTOK;
    }

    state->value = (Vec3d){
        state->x.value,
        state->y.value,
        state->z.value
    };

    return INTEGRAL_3D_OK;
}
