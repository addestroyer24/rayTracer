#ifndef _BOUNDING_BOX_H
#define _BOUNDING_BOX_H

#include "Ray.h"
#include "rayIntersectionInfo.h"
#include "Surface.h"

#include "libs/Matrix.h"

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

using surfaceBounds = std::pair<Vec3, Vec3>;

class BoundingBox : public Surface
{
private:
    Vec3 min;
    Vec3 max;
    Vec3 centroid;
    bool finalized = false;

    std::vector<Surface*> subSurfaces;

public:
    BoundingBox();
    virtual ~BoundingBox();

    void encompass(Surface* surf);
    void finalizeTree();

    bool hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record);

    virtual Vec3 getCentroid();
    virtual surfaceBounds getSurfaceBounds();
};

// BoundingBox::BoundingBox(Vec3 min, Vec3 max)
//     : min(min), max(max)
// {}

BoundingBox::BoundingBox()
    : Surface(""), min(0), max(0), centroid(0)
{}

BoundingBox::~BoundingBox()
{
    for (auto iter = this->subSurfaces.cbegin(); iter < this->subSurfaces.cend(); iter++)
    {
        delete *iter;
    }
}

void BoundingBox::encompass(Surface* surf)
{
    if (this->finalized)
    {
        std::cout << "Tried to expand a bounding box after it was finalized!" << std::endl;
        delete surf;
        return;
    }

    this->subSurfaces.push_back(surf);
    
    surfaceBounds bounds = surf->getSurfaceBounds();

    if (this->subSurfaces.size() > 1)
    {
        for (int i = 0; i < 3; i++)
        {
            this->min[i] = std::min(this->min[i], bounds.first[i]);
            this->max[i] = std::max(this->max[i], bounds.second[i]);
        }
    }
    else
    {
        this->min = bounds.first;
        this->max = bounds.second;
    }

    this->centroid += surf->getCentroid();
}

void BoundingBox::finalizeTree()
{
    if (this->finalized)
        return;
    
    this->finalized = true;

    if (this->subSurfaces.size() == 1)
        return;

    this->centroid = this->centroid / this->subSurfaces.size();

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

    for (auto iter = this->subSurfaces.begin(); iter < this->subSurfaces.end(); iter++)
    {
        Surface* surf = *iter;

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

    this->subSurfaces.clear();

    BoundingBox* leftBox = new BoundingBox();
    BoundingBox* rightBox = new BoundingBox();

    this->subSurfaces.push_back(leftBox);
    this->subSurfaces.push_back(rightBox);

    for (int i = 0; i < left.size(); i++)
    {
        leftBox->encompass(left[i]);
    }
    //left.clear();
    leftBox->finalizeTree();

    for (int i = 0; i < right.size(); i++)
    {
        rightBox->encompass(right[i]);
    }
    //right.clear();
    rightBox->finalizeTree();
}

bool BoundingBox::hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record)
{
    if (this->subSurfaces.size() == 0)
        return false;
/*
    float minXt, maxXt, minYt, maxYt, minZt, maxZt;

    Vec3 origin = ray.positionAtTime(0);
    Vec3 direction = ray.getDirection();

    minXt = getIntersectionTime(this->min, origin, direction, 0);
    maxXt = getIntersectionTime(this->max, origin, direction, 0);

    if (minXt > maxXt)
        std::swap(minXt, maxXt);

    if (minXt > endTime || maxXt < startTime)
        return false;

    minYt = getIntersectionTime(this->min, origin, direction, 1);
    maxYt = getIntersectionTime(this->max, origin, direction, 1);

    if (minYt > maxYt)
        std::swap(minYt, maxYt);

    if (minYt > endTime || maxYt < startTime)
        return false;

    minZt = getIntersectionTime(this->min, origin, direction, 2);
    maxZt = getIntersectionTime(this->max, origin, direction, 2);

    if (minZt > maxZt)
        std::swap(minZt, maxZt);

    if (minZt > endTime || maxZt < startTime)
        return false;

    int xNorm = 1, yNorm = 1, zNorm = 1;

    //if (this->max[0] - this->min[0] < 0.00001f)
        xNorm = ray.getDirection()[0] < 0 ? 1 : -1;

    //if (this->max[1] - this->min[1] < 0.00001f)
        yNorm = ray.getDirection()[1] < 0 ? 1 : -1;

    //if (this->max[2] - this->min[2] < 0.00001f)
        zNorm = ray.getDirection()[2] < 0 ? 1 : -1;

    float minMaxt = std::min({maxXt, maxYt, maxZt});

    // x is smallest
    if (minXt < minYt)
    {
        // z is largest
        if (minYt < minZt)
        {
            if (minZt > minMaxt)
                return false;
            record.intersectionTime = minZt;
            record.intersectionPoint = ray.positionAtTime(minZt);
            record.surfaceNormal = Vec3{0, 0, 1} * zNorm;
        }
        // y is largest
        else
        {
            if (minYt > minMaxt)
                return false;
            record.intersectionTime = minYt;
            record.intersectionPoint = ray.positionAtTime(minYt);
            record.surfaceNormal = Vec3{0, 1, 0} * yNorm;
        }
    }
    // y is smallest
    else
    {
        // z is largest
        if (minXt < minZt)
        {
            // hit z-plane first
            if (minZt > minMaxt)
                return false;
            record.intersectionTime = minZt;
            record.intersectionPoint = ray.positionAtTime(minZt);
            record.surfaceNormal = Vec3{0, 0, 1} * zNorm;
        }
        // x is largest
        else
        {
            if (minXt > minMaxt)
                return false;
            record.intersectionTime = minXt;
            record.intersectionPoint = ray.positionAtTime(minXt);
            record.surfaceNormal = Vec3{1, 0, 0} * xNorm;
        }
    }
    
    return true;
/*/
    //we want to find the farthest entrace and closest exit to the box
	//if the exit is closer than the entrance, there is no hit
	float entrance = startTime;
	float exit = endTime;
	Vec3 normal = Vec3{0,0,0};
	
	for(int i=0; i<3; i++)
	{
		float slabA = this->min[i];
		float slabB = this->max[i];
		float invDir = 1.0f / ray.getDirection()[i];
		float origin = ray.positionAtTime(0)[i];
		
		float closestHit = (slabA - origin) * invDir;
		float farthestHit = (slabB - origin) * invDir;
		
		if(farthestHit < closestHit)
			std::swap(closestHit, farthestHit);
		
		bool foundNewEntrance = closestHit > entrance;
		entrance = foundNewEntrance ? closestHit : entrance;
		
		bool foundNewExit = farthestHit < exit;
		exit = foundNewExit ? farthestHit : exit;
		
		bool tooClose = farthestHit < entrance;
		bool tooFar = closestHit > exit;
		
		if(tooClose || tooFar)
			return false;
		
		if(foundNewEntrance)
		{
            normal = Vec3(0);
			normal[i] = ray.getDirection()[i] < 0 ? 1 : -1;
		}
	}
	
    if (this->subSurfaces.size() == 1 && true)
    {
        record.surfaceNormal = normal;
        record.intersectionTime = entrance;
        record.intersectionPoint = ray.positionAtTime(entrance);
        return true;
    }

    bool hitSurface = false;
    rayIntersectionInfo newInfo;

    record.intersectionTime = endTime;

    for (auto iter = this->subSurfaces.begin(); iter != this->subSurfaces.end(); iter++)
    {
        if ((*iter)->hit(ray, startTime, endTime, newInfo))
        {
            record = newInfo;
            endTime = record.intersectionTime;
            hitSurface = true;
        }
    }
    return hitSurface;
//*/
}

Vec3 BoundingBox::getCentroid()
{
    return this->centroid;
}

surfaceBounds BoundingBox::getSurfaceBounds()
{
    return surfaceBounds(this->min, this->max);
}

#endif
