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

	static bool hit(float minMax[6], Ray ray, float startTime, rayHit *record);
};

bool BoundingBox::hit(float minMax[6], Ray ray, float startTime, rayHit *record)
{
    //we want to find the farthest entrace and closest exit to the box
	//if the exit is closer than the entrance, there is no hit
	float entrance = startTime;
	float exit = record->intersectionTime;
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
		
		if (exit < entrance)
			return false;
		
#ifdef RENDER_LEAF_BBOX
		if(foundNewEntrance)
		{
            normal = Vec3(0);
			normal[i] = dir[i] < 0 ? 1.0f : -1.0f;
		}
#endif
	}

#ifdef RENDER_LEAF_BBOX
    record->materialID = "";
    record->surfaceNormal = normal;
    record->intersectionTime = entrance;
    record->intersectionPoint = ray.positionAtTime(entrance);
#endif

    return true;
}

#endif
