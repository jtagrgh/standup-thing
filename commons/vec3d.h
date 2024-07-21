#ifndef VEC3D_H
#define VEC3D_H

#include <stdint.h>

typedef struct Vec3d {
    double x;
    double y;
    double z;
} Vec3d;

void vec3d_print(Vec3d a);

Vec3d vec3d_cross_product(Vec3d a, Vec3d b);
double vec3d_dot_product(Vec3d a, Vec3d b);
Vec3d vec3d_add_vectors(Vec3d a, Vec3d b);
Vec3d vec3d_sub_vectors(Vec3d a, Vec3d b);
Vec3d vec3d_add_scalar(Vec3d a, double scalar);
Vec3d vec3d_multiply_scalar(Vec3d a, double scalar);
double vec3d_magnitude(Vec3d a);
double vec3d_component(Vec3d a, Vec3d b);
Vec3d vec3d_project(Vec3d a, Vec3d b);
Vec3d vec3d_rotate_vector(Vec3d a, Vec3d axis, double angle);
Vec3d vec3d_rotate_about_x(Vec3d a, double angle);
Vec3d vec3d_rotate_about_y(Vec3d a, double angle);
Vec3d vec3d_rotate_about_z(Vec3d a, double angle);
Vec3d vec3d_rotate_by_rotation_vector_assuming_things_that_arent_true(Vec3d a, Vec3d rotation_vector);

#endif
