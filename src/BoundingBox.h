#ifndef _BOUNDING_BOX_H
#define _BOUNDING_BOX_H

#include "libs/Matrix.h"

class BoundingBox
{
private:
    Vec3 min;
    Vec3 max;

public:
    BoundingBox(Vec3 min, Vec3 max);
};

BoundingBox::BoundingBox(Vec3 min, Vec3 max)
    : min(min), max(max)
{}
#endif
