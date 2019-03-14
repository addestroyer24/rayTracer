#ifndef _COMPOSITE_SURFACE_H
#define _COMPOSITE_SURFACE_H

#include "Surface.h"

#include "rayIntersectionInfo.h"

#include <limits>
#include <vector>

class CompositeSurface : public Surface
{
private:
    std::vector<Surface*> surfaces;

public:
    CompositeSurface(int materialID);
    ~CompositeSurface();

    virtual bool hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record);

    void addSurface(Surface *surf);
    //int getMaterialID();
};

CompositeSurface::CompositeSurface(int materialID)
    : Surface(materialID)
{}

bool CompositeSurface::hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record)
{
    bool hitSurface = false;
    rayIntersectionInfo newInfo;

    record.intersectionTime = std::numeric_limits<int>::max();

    for (auto iter = this->surfaces.begin(); iter != this->surfaces.end(); iter++)
    {
        if ((*iter)->hit(ray, 0, 1000, newInfo) && newInfo.intersectionTime < record.intersectionTime)
        {
            record = newInfo;
            hitSurface = true;
        }
    }
    return hitSurface;
}

void CompositeSurface::addSurface(Surface *surf)
{
    this->surfaces.push_back(surf);
}

CompositeSurface::~CompositeSurface()
{
    for (auto iter = surfaces.begin(); iter != surfaces.end(); iter++)
    {
        delete *iter;
    }
}

#endif
