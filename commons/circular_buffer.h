#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>

typedef struct CircularBuffer {
    uint16_t size;
    uint16_t tail;
    double *data;
} CircularBuffer;

void init_circular_buffer(CircularBuffer *buffer, uint16_t size);
void add_to_circular_buffer(CircularBuffer *buffer, double value);
void free_circular_buffer(CircularBuffer *buffer);

#endif