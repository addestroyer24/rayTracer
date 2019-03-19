#ifndef _SPHERE_H
#define _SPHERE_H

#include "Surface.h"

#include "rayIntersectionInfo.h"

#include "libs/Matrix.h"

#include <math.h>
#include <string>

class Sphere : public Surface
{
private:
    Vec3 center;
    Vec3 equatorNormal;
    Vec3 upNormal;
    float radius;

public:
    Sphere(Vec3 center, Vec3 equatorNormal, Vec3 upNormal, float radius, std::string materialID);

    virtual bool hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record);
    //virtual BoundingBox getBoundingBox() = 0;
};

Sphere::Sphere(Vec3 center, Vec3 equatorNormal, Vec3 upNormal, float radius, std::string materialID)
    : Surface(materialID), center(center), radius(radius)
{
    this->equatorNormal = Mat::normalize(equatorNormal);
    this-> upNormal = Mat::normalize(upNormal);
}

bool Sphere::hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record)
{
    Vec3 d = ray.getDirection();
    Vec3 e = ray.positionAtTime(0);
    float dDotemc = Mat::dot(d, e - this->center);
    float dDotd = Mat::dot(d, d);
    float discriminant = dDotemc * dDotemc - dDotd * (Mat::dot(e - this->center, e - this->center) - this->radius * this->radius);

    if (discriminant < 0)
        return false;

    discriminant = sqrtf(discriminant);
    float time;

    if ((-dDotemc - discriminant) > 0)
    {
        time = (-dDotemc - discriminant) / dDotd;
        //Hit the far side of the sphere
    }
    else
    {
        time = (-dDotemc + discriminant) / dDotd;
        //Hit the near side of the sphere
    }

    if (!(startTime < time && time < endTime))
        return false;

    record.intersectionTime = time;
    record.intersectionPoint = ray.positionAtTime(time);
    record.surfaceNormal = Mat::normalize(record.intersectionPoint - this->center);
    record.materialID = this->materialName;
    
    return true;
}

#endif 
