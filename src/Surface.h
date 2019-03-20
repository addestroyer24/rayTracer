#ifndef _SURFACE_H
#define _SURFACE_H

#include "BoundingBox.h"
#include "Ray.h"
#include "rayIntersectionInfo.h"

#include <string>

class Surface
{
protected:
    std::string materialName;

public:
    Surface(std::string materialID);
    virtual bool hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record) = 0;
    virtual BoundingBox getBoundingBox() = 0;

    virtual ~Surface() = default;
};

Surface::Surface(std::string materialID)
    : materialName(materialID)
{}

#endif 
