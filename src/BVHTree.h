#ifndef _BVH_TREE_H
#define _BVH_TREE_H

#include "BoundingBox.h"
#include "Ray.h"
#include "rayIntersectionInfo.h"
#include "Surface.h"

#include "libs/Matrix.h"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

class BVHTree : public Surface
{
private:
    Vec3 min;
    Vec3 max;
    Vec3 centroid;

    std::vector<Surface*> subSurfaces;

public:
    BVHTree(std::vector<Surface*>&);
    virtual ~BVHTree();

    bool hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record);

    virtual Vec3 getCentroid();
    virtual BoundingBox getBoundingBox();
};

BVHTree::BVHTree(std::vector<Surface*>& surfaces)
    : Surface(""), min(0), max(0), centroid(0)
{
    this->min = surfaces[0]->getBoundingBox().min;
    this->max = surfaces[0]->getBoundingBox().max;

    this->centroid = surfaces[0]->getCentroid();

    for (int i = 1; i < surfaces.size(); i++)
    {
        BoundingBox bounds = surfaces[i]->getBoundingBox();
        for (int j = 0; j < 3; j++)
        {
            this->min[j] = std::min(this->min[j], bounds.min[j]);
            this->max[j] = std::max(this->max[j], bounds.max[j]);
        }

        this->centroid += surfaces[i]->getCentroid();
    }

    if (surfaces.size() == 1)
    {
        this->subSurfaces.push_back(surfaces[0]);
        return;
    }

    this->centroid = this->centroid / surfaces.size();

    float maxDimWidth = 0;
    int maxDim;
    
    for (int i = 0; i < 3; i++)
    {
        if (this->max[i] - this->min[i] > maxDimWidth)
        {
            maxDimWidth = this->max[i] - this->min[i];
            maxDim = i;
        }
    }

    float splitPoint = (this->max[maxDim] + this->min[maxDim]) / 2;
    splitPoint = this->centroid[maxDim];

    std::vector<Surface*> left, right;

    for (auto *surf : surfaces)
    {
        if (surf->getCentroid()[maxDim] < splitPoint)
        {
            left.push_back(surf);
        }
        else
        {
            right.push_back(surf);
        }
    }

    if (left.size() == 0)
    {
        left.push_back(right.back());
        right.pop_back();
    }
    if (right.size() == 0)
    {
        right.push_back(left.back());
        left.pop_back();
    }

    this->subSurfaces.push_back(new BVHTree(left));
    this->subSurfaces.push_back(new BVHTree(right));
}

BVHTree::~BVHTree()
{
    for (auto *surf : this->subSurfaces)
    {
        delete surf;
    }
}

bool BVHTree::hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record)
{
    if (this->subSurfaces.size() == 0)
        return false;

    bool hitSurface = false;
    rayIntersectionInfo newInfo;

    hitSurface = BoundingBox(this->min, this->max).hit(ray, startTime, endTime, newInfo);

    if (!hitSurface)
        return false;

    hitSurface = false;
	
    // toggle the boolean to determine whether to draw leaf bounding boxes instead of primitives
    if (this->subSurfaces.size() == 1 && false)
    {
        record = newInfo;
        return true;
    }

    record.intersectionTime = endTime;

    for (auto *surf : this->subSurfaces)
    {
        if (surf->hit(ray, startTime, endTime, newInfo))
        {
            record = newInfo;
            endTime = record.intersectionTime;
            hitSurface = true;
        }
    }
    return hitSurface;
}

Vec3 BVHTree::getCentroid()
{
    return this->centroid;
}

BoundingBox BVHTree::getBoundingBox()
{
    return BoundingBox(this->min, this->max);
}

#endif
