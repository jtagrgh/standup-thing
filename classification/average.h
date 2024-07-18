#ifndef AVERAGE_H
#define AVERAGE_H

typedef enum AverageStatus {
    AVERAGE_OK,
    AVERAGE_NOTOK
} AverageStatus;

typedef struct AverageState {
    uint16_t n_samples; /* Number of samples */
    uint16_t value; /* Average value */
} AverageState;

#endif