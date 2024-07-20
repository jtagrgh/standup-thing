#include "average.h"
#include "stdio.h"

void init_average_state(AverageState *state, uint8_t debug) {
    state->base.debug = debug;
    state->n_samples = 0;
    state->value = 0;
}

AverageStatus average(AverageState *state, double new_sample) {
    state->n_samples++;
    double alpha = 1/(double)state->n_samples;
    state->value += alpha*(new_sample - state->value);

    if (state->base.debug) {
        printf("%f, ", state->value);
    }

    return AVERAGE_OK;
} 
