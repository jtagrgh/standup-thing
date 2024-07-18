#include "analyze.h"
#include "average.h"

AnalysisStatus analyze(BMI2SensData data[], uint16_t n, BMI2SensAxisData g, AnalysisResult *out) {

    AdjustAccelerationState acc_adjusted;
    init_adjust_acceleration_state(&acc_adjusted);
    
    AdjustGyroState gyr_adjusted;
    init_adjust_gyro_state(&gyr_adjusted);
    
    IntegralState rotation_vector;
    init_integral_state(&rotation_vector);
    
    RotationMatrixState rotation_matrix;
    init_rotation_matrix_state(&rotation_matrix);
    
    MatrixMultiplyState gravity_adjusted;
    init_matrix_multiply_state(&gravity_adjusted);
    
    ComponentVectorState vertical_acc;
    init_component_vector_state(&vertical_acc);
    
    IntegralState vertical_vel;
    init_integral_state(&vertical_vel);
    
    IntegralState vertical_pos;
    init_integral_state(&vertical_pos);
    
    AverageState average_vertical_pos;
    init_average_state(&average_vertical_pos);
    
    for (int i = 0; i < n; i++) {
        /* Adjust the raw acceleration vector. */
        adjust_acceleration_state(&acc_adjusted, data[i].acc);
        
        /* Adjust the raw gyro vector. */
        adjust_acceleration_state(&gyr_adjusted, data[i].gyr);
        
        /* Integrate for the gyro position. */
        integral(&rotation_vector, gyr_adjusted.value);
        
        /* Get the gyro rotation matrix */
        rotation_matrix(&rotation_matrix, rotation_vector.value);
        
        /* Adjust gravity with the rotation matrix. */
        matrix_multiply(&gravity_adjusted, g, rotation_matrix.value);
        
        /* Get the vertical (gravity direction) component of acceleration. */
        component_vector(&vertical_acc, acc_adjusted.value, gravity_adjusted.value);
        
        /* Integrate for vertical velocity. */
        integral(&vertical_vel, vertical_acc.value);
        
        /* Integrate for vertical position */
        integral(&vertical_pos, vertical_vel.value);
        
        /* Maintain the average vertical position over the data */
        average(&average_vertical_pos, vertical_pos.value);
    }
    
    /* Classify. */
    out->classification = classify(average_vertical_pos.value);
    
    return OK;
}