#include "adjust_acceleration.h"
#include <stdio.h>


void init_adjust_acceleration_state(AdjustAccelerationState *state) {
    state->value.x = 0;
    state->value.y = 0;
    state->value.z = 0;
}

static double convert_axis_value(int16_t value) {
    return (double)value / 16384 * 9.80665;
}

AdjustAccelerationStatus adjust_acceleration(AdjustAccelerationState *state, BMI2SensAxisData new_sample) {
    state->value.x = convert_axis_value(new_sample.x);
    state->value.y = convert_axis_value(new_sample.y);
    state->value.z = convert_axis_value(new_sample.z);

    if (state->base.debug) {
        printf("%s_x:%f,%s_y:%f,%s_z:%f,", state->base.name, state->value.x, state->base.name, state->value.y, state->base.name, state->value.z);
    }

    return ADJUST_ACCELERATION_OK;
}
