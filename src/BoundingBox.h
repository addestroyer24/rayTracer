#ifndef _BOUNDING_BOX_H
#define _BOUNDING_BOX_H

#include "Ray.h"
#include "rayHit.h"

class BoundingBox
{
public:
    float minMax[6];

    BoundingBox() = default;
    BoundingBox(Vec3 min, Vec3 max) 
		//: min(min), max(max) 
	{
		for (int i = 0; i < 3; i++)
		{
			minMax[i] = min[i];
			minMax[i+3] = max[i];
		}
	}
	BoundingBox(float vals[6])
	{
		for (int i = 0; i < 6; i++)
		{
			minMax[i] = vals[i];
		}
	}

    bool hit(Ray ray, float startTime, float endTime, rayHit &info, float *exitTime = nullptr)
	{
		return hit(this->minMax, ray, startTime, endTime, info, exitTime);
	}
	static bool hit(float minMax[6], Ray ray, float startTime, float endTime, rayHit &info, float *exitTime = nullptr);
};

bool BoundingBox::hit(float minMax[6], Ray ray, float startTime, float endTime, rayHit &record, float *exitTime)
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
		float slabA = minMax[i];
		float slabB = minMax[i+3];
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
