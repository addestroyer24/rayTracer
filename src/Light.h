#ifndef _LIGHT_H
#define _LIGHT_H

#include "libs/Matrix.h"

class Light
{
private:
    Vec3 position;
    int materialID;

public:
    Light(Vec3 position, int material);

    Vec3 getPosition();
    int getMaterial();
};

Light::Light(Vec3 position, int material)
    : position(position), materialID(material)
{}

Vec3 Light::getPosition()
{
    return this->position;
}

int Light::getMaterial()
{
    return this->materialID;
}

#endif
