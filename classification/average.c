#include "average.h"

void init_average_state(AverageState *state) {
    state->n_samaples = 0;
    state->value = 0;
}

AverageStatus average(AverageState *state, uint16_t new_sample) {
    state->n_samples++;
    uint16_t alpha = 1/state->n_samples;
    state->value += alpha*(new_sample - state->value);
    return AVERAGE_OK;
} 
