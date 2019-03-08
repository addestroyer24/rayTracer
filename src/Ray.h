#ifndef _RAY_H
#define _RAY_H

#include "libs/Matrix.h"

class Ray
{
private:
    Vec3 origin;
    Vec3 direction;

public:
    Ray(Vec3 origin, Vec3 direction);

    Vec3 getDirection();
    Vec3 getOrigin();
};

Ray::Ray(Vec3 origin, Vec3 direction)
    : origin(origin), direction(direction)
{
}

Vec3 Ray::getDirection()
{
    return this->direction;
}

Vec3 Ray::getOrigin()
{
    return this->origin;
}

#endif