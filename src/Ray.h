#ifndef _RAY_H
#define _RAY_H

#include "libs/GenVector.h"

class Ray
{
private:
    Vector3 origin;
    Vector3 direction;

public:
    Ray(Vector3 origin, Vector3 direction);

    Vector3 getDirection();
};

Ray::Ray(Vector3 origin, Vector3 direction)
    : origin(origin), direction(direction)
{
}

Vector3 Ray::getDirection()
{
    return this->direction;
}

#endif