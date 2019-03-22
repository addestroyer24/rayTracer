#ifndef _SURFACE_H
#define _SURFACE_H

// #include "BoundingBox.h"
using surfaceBounds = std::pair<Vec3, Vec3>;
#include "Ray.h"
#include "rayIntersectionInfo.h"

#include <string>

class Surface
{
protected:
    std::string materialName;

public:
    Surface(std::string materialID);
    virtual ~Surface() = default;

    virtual bool hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record) = 0;

    virtual Vec3 getCentroid() = 0;
    virtual surfaceBounds getSurfaceBounds() = 0;
};

Surface::Surface(std::string materialID)
    : materialName(materialID)
{}

#endif 
