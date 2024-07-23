#include "quaternion.h"
#include <math.h>
#include <stdio.h>

Quaternion quaternion_product(Quaternion q, Quaternion r){
    double qw = q.w*r.w - q.x*r.x - q.y*r.y - q.z*r.z;
    double qx = q.x*r.w + q.w*r.x - q.z*r.y + q.y*r.z;
    double qy = q.y*r.w + q.z*r.x + q.w*r.y - q.x*r.z;
    double qz = q.z*r.w - q.y*r.x + q.x*r.y + q.w*r.z;

    return (Quaternion){qw,qx,qy,qz};

}