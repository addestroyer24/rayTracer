#ifndef _SURFACE_H
#define _SURFACE_H

#include "Ray.h"
#include "rayIntersectionInfo.h"

class Surface
{
public:
    virtual bool hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record) = 0;
    //virtual BoundingBox getBoundingBox() = 0;
};

#endif 
