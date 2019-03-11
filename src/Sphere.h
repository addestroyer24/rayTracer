#ifndef _SPHERE_H
#define _SPHERE_H

#include "rayIntersectionInfo.h"
#include "Surface.h"

#include "libs/Matrix.h"

#include <math.h>

class Sphere : public Surface
{
private:
    Vec3 center;
    Vec3 equatorNormal;
    Vec3 upNormal;
    float radius;

public:
    Sphere(Vec3 center, Vec3 equatorNormal, Vec3 upNormal, float radius);

    virtual bool hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record);
    //virtual BoundingBox getBoundingBox() = 0;
};

Sphere::Sphere(Vec3 center, Vec3 equatorNormal, Vec3 upNormal, float radius)
    : center(center), radius(radius)
{
    this->equatorNormal = Mat::normalize(equatorNormal);
    this-> upNormal = Mat::normalize(upNormal);
}

bool Sphere::hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record)
{
    Vec3 d = ray.getDirection();
    Vec3 e = ray.getOrigin();
    float dDotemc = Mat::dot(d, e - this->center);
    float dDotd = Mat::dot(d, d);
    float discriminant = dDotemc * dDotemc - dDotd * (Mat::dot(e - this->center, e - this->center) - this->radius * this->radius);

    if (discriminant < 0)
        return false;

    discriminant = sqrtf(discriminant);

    if (-dDotemc - discriminant < 0)
    {
        record.intersectionTime = (-dDotemc - discriminant) / dDotd;
        //Hit the far side of the sphere
    }
    else
    {
        record.intersectionTime = (-dDotemc + discriminant) / dDotd;
        //Hit the near side of the sphere
    }
    
    return true;
}

#endif 
