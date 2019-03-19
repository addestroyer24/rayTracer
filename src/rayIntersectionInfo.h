#ifndef _RAY_INTERSECTION_INFO_H
#define _RAY_INTERSECTION_INFO_H

#include "libs/Matrix.h"

#include <string>

struct rayIntersectionInfo
{
    float intersectionTime;
    Vec3 intersectionPoint;
    Vec3 surfaceNormal;
    std::string materialID;
    //Material
};

#endif
