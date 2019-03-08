#ifndef _SPHERE_H
#define _SPHERE_H

#include "libs/Matrix.h"
#include "Surface.h"

class Sphere : public Surface
{
private:
    Vec3 center;
    float radius;

public:
    Sphere(Vec3 center, float radius);

    virtual bool hit(Ray ray, float startTime, float endTime/*, hitRecord record */);
    //virtual BoundingBox getBoundingBox() = 0;
};

Sphere::Sphere(Vec3 center, float radius)
    : center(center), radius(radius)
{}

bool Sphere::hit(Ray ray, float startTime, float endTime/*, hitRecord record */)
{
    Vec3 d = ray.getDirection();
    Vec3 e = ray.getOrigin();
    float dDotemc = Mat::dot(d, e - this->center);
    float dDotd = Mat::dot(d, d);
    float discriminant = dDotemc * dDotemc - dDotd * (Mat::dot(e - this->center, e - this->center) - this->radius * this->radius);

    if (discriminant < 0)
        return false;

    if (-dDotemc - discriminant < 0)
    {
        //Hit the far side of the sphere
    }
    else
    {
        //Hit the near side of the sphere
    }
    
    return true;
}

#endif 
