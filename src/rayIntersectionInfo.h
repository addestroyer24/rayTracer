#ifndef _RAY_INTERSECTION_INFO_H
#define _RAY_INTERSECTION_INFO_H

#include "libs/Matrix.h"

struct rayIntersectionInfo
{
    float intersectionTime;
    Vec3 intersectionPoint;
    Vec3 surfaceNormal;
    int materialID;
    //Material
};

#endif
