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

    Vec3d g_start = bmisensdata_to_vec3d(data[0].acc);
    Vec3d g_end = bmisensdata_to_vec3d(data[data_length - 1].acc);
    Vec3d g_average = vec3d_multiply_scalar(vec3d_add_vectors(g_start, g_end), (double)1/2);
    g.acc = vec3d_to_bmisensdata(g_average);

    AdjustAccelerationState acc_adjusted;
    init_analysis_functor(&acc_adjusted.base, debug, "acc_adjusted");
    init_adjust_acceleration_state(&acc_adjusted);

    AdjustAccelerationState g_adjusted;
    init_analysis_functor(&g_adjusted.base, debug, "g_adjusted");
    init_adjust_acceleration_state(&g_adjusted);

    AdjustGyroState gyr_adjusted;
    init_analysis_functor(&gyr_adjusted.base, debug, "gyr_adjusted");
    init_adjust_gyro_state(&gyr_adjusted);
    
    Integral3dState rotation_vector;
    init_analysis_functor(&rotation_vector.base, debug, "rotation_vector");
    init_integral_3d_state(&rotation_vector, SAMPLE_PERIOD);
    
    RotateVectorState gravity_adjusted;
    init_analysis_functor(&gravity_adjusted.base, debug, "gravity_adjusted");
    init_rotation_vector_state(&gravity_adjusted);
    
    VectorComponentState vertical_acc;
    init_analysis_functor(&vertical_acc.base, debug, "vertical_acc");
    init_vector_component_state(&vertical_acc);
    
    IntegralState vertical_vel;
    init_analysis_functor(&vertical_vel.base, debug, "vertical_vel");
    init_integral_state(&vertical_vel, SAMPLE_PERIOD);
    
    IntegralState vertical_pos;
    init_analysis_functor(&vertical_pos.base, debug, "vertical_pos");
    init_integral_state(&vertical_pos, SAMPLE_PERIOD);
    
    AverageState average_vertical_pos;
    init_analysis_functor(&average_vertical_pos.base, debug, "average_vertical_pos");
    init_average_state(&average_vertical_pos);

    AddState vertical_acc_normalized;
    init_analysis_functor(&vertical_acc_normalized.base, debug, "vertical_acc_normalized");
    init_add_state(&vertical_acc_normalized);

    AverageState average_vertical_acc;
    init_analysis_functor(&average_vertical_acc.base, debug, "average_vertical_acc");
    init_average_state(&average_vertical_acc);

    double raw_vertical_acc[data_length];

    
    int i;
    for (i = 0; i < data_length; i++) {
        /* Adjust the raw acceleration vector. */
        adjust_acceleration(&acc_adjusted, data[i].acc);

        /* Adjust the raw gravity vector. */
        adjust_acceleration(&g_adjusted, g.acc);
        
        /* Adjust the raw gyro vector. */
        // adjust_gyro(&gyr_adjusted, data[i].gyr);
        
        /* Integrate for the gyro position. */
        // integral_3d(&rotation_vector, gyr_adjusted.value);

        /* Rotate the gravity vector with the rotation vector. (This is not done well) */
        // rotate_vector(&gravity_adjusted, g_adjusted.value, rotation_vector.value);
        
        /* Get the vertical (gravity direction) component of acceleration. */
        vector_component(&vertical_acc, acc_adjusted.value, g_adjusted.value);

        /* Maintain average raw acceleration. */
        average(&average_vertical_acc, vertical_acc.value);

        /* Store the raw vertical acceleration. */
        raw_vertical_acc[i] = vertical_acc.value;
    }

    for (i = 0; i < data_length; i++) {
        /* Normalize the vertical acceleration. */
        add(&vertical_acc_normalized, raw_vertical_acc[i], -average_vertical_acc.value);

        /* Integrate for vertical velocity */
        integral(&vertical_vel, vertical_acc_normalized.value);

        /* Integrate for vertical position */
        integral(&vertical_pos, vertical_vel.value);

        /* Maintain the average vertical position over the data */
        average(&average_vertical_pos, vertical_pos.value);
    }
    
    /* Classify. */
    out->classification = classify(average_vertical_pos.value);
    
    return ANALYSIS_OK;
}
