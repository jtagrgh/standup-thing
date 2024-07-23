#include "vec3d.h"
#include "quaternion.h"
#include <math.h>
#include <stdio.h>

void vec3d_print(Vec3d a) {
    printf("<%f %f %f>\n", a.x, a.y, a.z);
}

Vec3d vec3d_cross_product(Vec3d a, Vec3d b)
{
    return (Vec3d) {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

double vec3d_dot_product(Vec3d a, Vec3d b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double vec3d_component(Vec3d a, Vec3d b) {
    return vec3d_dot_product(a, b) / vec3d_magnitude(b);
}

Vec3d vec3d_add_vectors(Vec3d a, Vec3d b) {
    return (Vec3d) {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

Vec3d vec3d_add_scalar(Vec3d a, double scalar) {
    return (Vec3d) {
        a.x + scalar,
        a.y + scalar,
        a.z + scalar
    };
}

Vec3d vec3d_multiply_scalar(Vec3d a, double scalar) {
    return (Vec3d) {
        a.x * scalar,
        a.y * scalar,
        a.z * scalar
    };
}

double vec3d_magnitude(Vec3d a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

Vec3d vec3d_project(Vec3d a, Vec3d b) { /* a onto b*/
    Vec3d b_unit_vector = vec3d_multiply_scalar(b, 1 / vec3d_magnitude(b)); 
    double component_a_onto_b = vec3d_dot_product(a, b) / vec3d_magnitude(b);

    return vec3d_multiply_scalar(b_unit_vector, component_a_onto_b);
}

Vec3d vec3d_rotate_vector(Vec3d a, Vec3d axis, double angle) {
    Vec3d term1 = vec3d_multiply_scalar(vec3d_project(a, axis), (1 - cos(angle)));
    Vec3d term2 = vec3d_multiply_scalar(a, cos(angle));
    Vec3d term3 = vec3d_multiply_scalar(vec3d_cross_product(axis, a), sin(angle));

    return vec3d_add_vectors(term1, vec3d_add_vectors(term2, term3));
}

Vec3d vec3d_rotate_about_x(Vec3d a, double angle) {
    return (Vec3d) {
        a.x,
        a.y * cos(angle) - a.z * sin(angle),
        a.y * sin(angle) + a.z * cos(angle)
    };
}

Vec3d vec3d_rotate_about_y(Vec3d a, double angle) {
    return (Vec3d) {
        a.x * cos(angle) + a.z * sin(angle),
        a.y,
        -a.x * sin(angle) + a.z * cos(angle)
    };
}

Vec3d vec3d_rotate_about_z(Vec3d a, double angle) {
    return (Vec3d) {
        a.x * cos(angle) - a.y * sin(angle),
        a.x * sin(angle) + a.y * cos(angle),
        a.z
    };
}

/*
Vec3d vec3d_rotate_by_quaternion(Vec3d a, Vec3d rotation_vector, double period){
    Vec3d rotated = a;
    //Quaternion product of (0, rotation_vector) x (0, rotated) 
    double r = -1 * vec3d_dot_product(rotation_vector, rotated);
    double x = -1 * rotation_vector.z * rotated.y   +  rotation_vector.y * rotated.z;
    double y = rotation_vector.z * rotated.x   -   rotation_vector.x * rotated.z;
    double z = -1 * rotation_vector.y * rotated.x + rotation_vector.x * rotated.y;

    //multiply by dt/2
    r *= (0.5) * (period);
    x *= (0.5) * (period);
    y *= (0.5) * (period);
    z *= (0.5) * (period);

    //summing the previous product and rotated
    x += rotated.x;
    y += rotated.y;
    z += rotated.z;

}
*/

Vec3d vec3d_rotate_by_quaternion(Vec3d a, Vec3d rotation_vector, double period){
    
    double cx = cos(rotation_vector.x * 0.5 * period);
    double sx = sin(rotation_vector.x * 0.5 * period);
    double cy = cos(rotation_vector.y * 0.5 * period);
    double sy = sin(rotation_vector.y * 0.5 * period);
    double cz = cos(rotation_vector.z * 0.5 * period);
    double sz = sin(rotation_vector.z * 0.5 * period);

    //convert rotation vector to a quaternion
    double qw  = cx*cy*cz + sx*sy*sz;
    double qx  = sx*cy*cz - cx*sy*sz;
    double qy  = cx*sy*cz + sx*cy*sz; 
    double qz  = cx*cy*sz - sx*sy*cz; 

    Quaternion q = (Quaternion){qw,qx,qy,qz};
    //conjugate
    Quaternion qn = (Quaternion){qw,-1*qx,-1*qy,-1*qz};
    //Target vector
    Quaternion rotated = (Quaternion){0, a.x, a.y, a.z};

    // qxrxq'
    Quaternion result = quaternion_product(quaternion_product(q,rotated), qn);

    return (Vec3d){result.x, result.y, result.z};

    


}


Vec3d vec3d_rotate_by_rotation_vector_assuming_things_that_arent_true(Vec3d a, Vec3d rotation_vector) {
    Vec3d rotated = a;
    rotated = vec3d_rotate_about_x(rotated, rotation_vector.x);
    rotated = vec3d_rotate_about_y(rotated, rotation_vector.y);
    rotated = vec3d_rotate_about_z(rotated, rotation_vector.z);

    return rotated;
}
