#ifndef QUATERNION_H
#define QUATERNION_H

#include <stdint.h>

typedef struct Quaternion {
    double w;
    double x;
    double y;
    double z;
} Quaternion;

Quaternion quaternion_product(Quaternion q, Quaternion r);


#endif