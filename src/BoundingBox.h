#ifndef _BOUNDING_BOX_H
#define _BOUNDING_BOX_H

#include "Ray.h"
#include "rayIntersectionInfo.h"

class BoundingBox
{
public:
    Vec3 min;
    Vec3 max;

    BoundingBox();
    BoundingBox(Vec3 min, Vec3 max) : min(min), max(max) {}

    bool hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &info, float *exitTime = nullptr);
};

bool BoundingBox::hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record, float *exitTime)
{
    //we want to find the farthest entrace and closest exit to the box
	//if the exit is closer than the entrance, there is no hit
	float entrance = startTime;
	float exit = endTime;
	Vec3 normal = Vec3(0);
	Vec3 dir = ray.getDirection();
	Vec3 org = ray.positionAtTime(0);
	
	for(int i=0; i<3; i++)
	{
		float slabA = this->min[i];
		float slabB = this->max[i];
		float invDir = 1.0f / dir[i];
		float origin = org[i];
		
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

        // if (foundNewExit && exitNormal != nullptr)
        // {
        //     *exitNormal = Vec3(0);
        //     (*exitNormal)[i] = ray.getDirection()[i] < 0 ? -1 : 1;
        // }
	}

    record.materialID = "";
    record.surfaceNormal = normal;
    record.intersectionTime = entrance;
    record.intersectionPoint = ray.positionAtTime(entrance);

    if (exitTime != nullptr)
        *exitTime = exit;

    return true;
}

#endif
