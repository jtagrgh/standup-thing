#include "analyze.h"
#include "../commons/vec3d.h"
#include "average.h"
#include "adjust_acceleration.h"
#include "adjust_gyro.h"
#include "integral.h"
#include "integral_3d.h"
#include "rotate_vector.h"
#include "vector_component.h"
#include "add.h"
#include "vec3d.h"
#include "../commons/constants.h"
#include <stdio.h>
#include <string.h>


void init_analysis_functor(AnalysisFunctor *functor, uint8_t debug, char name[ANALYSIS_FUNCTOR_NAME_LENGTH]) {
    functor->debug = debug;
    strcpy(functor->name, name);
}

AnalysisStatus analyze(
        BMI2SensData data[],
        uint16_t data_length,
        BMI2SensData g,
        uint8_t debug,
        AnalysisResult *out) {

    BMI2SensAxisData mean_g = {0, 0, 0};
    for (int i = 0; i < (data_length > 100 ? 100 : 1); i++) {
        int n = i + 1;
        mean_g.x += 1.0/n * (data[data_length - n - 1].acc.x - mean_g.x);
        mean_g.y += 1.0/n * (data[data_length - n - 1].acc.y - mean_g.y);
        mean_g.z += 1.0/n * (data[data_length - n - 1].acc.z - mean_g.z);
    }

    AdjustAccelerationState acc_adj;
    init_analysis_functor(&acc_adj.base, debug, "acc_adj");
    init_adjust_acceleration_state(&acc_adj);

    AdjustAccelerationState g_adj;
    init_analysis_functor(&g_adj.base, debug, "g_adj");
    init_adjust_acceleration_state(&g_adj);

    VectorComponentState vert_acc;
    init_analysis_functor(&vert_acc.base, debug, "vert_acc");
    init_vector_component_state(&vert_acc);

    IntegralState vert_vel;
    init_analysis_functor(&vert_vel.base, debug, "vert_vel");
    init_integral_state(&vert_vel, SAMPLE_PERIOD);

    IntegralState vert_pos;
    init_analysis_functor(&vert_pos.base, debug, "vert_pos");
    init_integral_state(&vert_pos, SAMPLE_PERIOD);

    AverageState avg_vert_pos;
    init_analysis_functor(&avg_vert_pos.base, debug, "avg_vert_pos");
    init_average_state(&avg_vert_pos);

    int i = 0;
    for (i = 0; i < data_length; i++) {
        adjust_acceleration(&acc_adj, data[i].acc);
        adjust_acceleration(&g_adj, mean_g);
        vector_component(&vert_acc, acc_adj.value, g_adj.value);
        integral(&vert_vel, vert_acc.value);
        integral(&vert_pos, vert_vel.value);
        average(&avg_vert_pos, vert_pos.value);
    }

    /* Classify. */
    out->classification = classify(avg_vert_pos.value);
    
    return ANALYSIS_OK;
}
