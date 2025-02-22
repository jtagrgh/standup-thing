#include "integral.h"
#include "constants.h"
#include "circular_buffer.h"

/*  Cummulative integral using four-points backwards rule. 
    https://ece.uwaterloo.ca/~dwharder/nm/Lecture_materials/pdfs/5.2.3%20Approximating%20integrals%20using%20interpolating%20polynomials.pdf
*/

static int16_t term_constants[] = {1, -5, 19, 9};

void init_integral_state(IntegralState *state, double interval) {
    state->n_samples = 0;
    state->value = 0;
    state->interval = interval;
    init_circular_buffer(&state->samples, 4);
}

void free_integral_state(IntegralState *state) {
    free_circular_buffer(&state->samples);
}

IntegralStatus integral(IntegralState *state, double new_sample) {
    state->n_samples++;
    add_to_circular_buffer(&state->samples, new_sample);

    if (state->n_samples < 4) {
        state->value += new_sample * state->interval;
        return INTEGRAL_OK;
    }

    double area = 0;
    for (int i = 0; i < 4; i++) {
        double sample_value = state->samples.data[(state->samples.tail + i) % state->samples.size];
        area += sample_value * term_constants[i];
    }
    area /= 24;
    area *= state->interval;

    state->value += area;

    return INTEGRAL_OK;
}
