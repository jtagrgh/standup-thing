#ifndef ADJUST_GYRO_H
#define ADJUST_GYRO_H

#include <stdint.h>
#include "vec3d.h"
#include "data_formats.h"

typedef enum AdjustGyroStatus {
    ADJUST_GYRO_OK,
    ADJUST_GYRO_NOTOK
} AdjustGyroStatus;

typedef struct AdjustGyroState {
    Vec3d value;
} AdjustGyroState;

void init_adjust_gyro_state(AdjustGyroState *state);

AdjustGyroStatus adjust_gyro(AdjustGyroState *state, BMI2SensAxisData new_sample);

#endif
