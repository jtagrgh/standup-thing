#include "analyze.h"
#include "../commons/vec3d.h"
#include "average.h"
#include "adjust_acceleration.h"
#include "adjust_gyro.h"
#include "integral.h"
#include "integral_3d.h"
#include "rotate_vector.h"
#include "vector_component.h"
#include "../commons/constants.h"

AnalysisStatus analyze(
        BMI2SensData data[],
        uint16_t data_length,
        BMI2SensData g,
        AnalysisResult *out) {

    AdjustAccelerationState acc_adjusted;
    init_adjust_acceleration_state(&acc_adjusted);

    AdjustAccelerationState g_adjusted;
    init_adjust_acceleration_state(&g_adjusted);
    
    AdjustGyroState gyr_adjusted;
    init_adjust_gyro_state(&gyr_adjusted);
    
    Integral3dState rotation_vector;
    init_integral_3d_state(&rotation_vector, SAMPLE_PERIOD);
    
    RotateVectorState gravity_adjusted;
    init_rotation_vector_state(&gravity_adjusted);
    
    VectorComponentState vertical_acc;
    init_vector_component_state(&vertical_acc);
    
    IntegralState vertical_vel;
    init_integral_state(&vertical_vel, SAMPLE_PERIOD);
    
    IntegralState vertical_pos;
    init_integral_state(&vertical_pos, SAMPLE_PERIOD);
    
    AverageState average_vertical_pos;
    init_average_state(&average_vertical_pos);
    
    int i;
    for (i = 0; i < data_length; i++) {
        /* Adjust the raw acceleration vector. */
        adjust_acceleration(&acc_adjusted, data[i].acc);

        /* Adjust the raw gravity vector. */
        adjust_acceleration(&g_adjusted, g.acc);
        
        /* Adjust the raw gyro vector. */
        adjust_gyro(&gyr_adjusted, data[i].gyr);
        
        /* Integrate for the gyro position. */
        integral_3d(&rotation_vector, gyr_adjusted.value);

        /* Rotate the gravity vector with the rotation vector. (This is not done well) */
        rotate_vector(&gravity_adjusted, g_adjusted.value, rotation_vector.value);
        
        /* Get the vertical (gravity direction) component of acceleration. */
        vector_component(&vertical_acc, acc_adjusted.value, gravity_adjusted.value);
        
        /* Integrate for vertical velocity. */
        integral(&vertical_vel, vertical_acc.value);
        
        /* Integrate for vertical position */
        integral(&vertical_pos, vertical_vel.value);
        
        /* Maintain the average vertical position over the data */
        average(&average_vertical_pos, vertical_pos.value);
    }
    
    /* Classify. */
    out->classification = classify(average_vertical_pos.value);
    
    return ANALYSIS_OK;
}
