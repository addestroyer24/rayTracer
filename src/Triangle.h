#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "Surface.h"

#include "libs/Matrix.h"

class Triangle : public Surface
{
private:
    Vec3 a;
    Vec3 b;
    Vec3 c;
    Vec3 normal;

public:
    Triangle(Vec3 a, Vec3 b, Vec3 c, int materialID);

    virtual bool hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record);
};

Triangle::Triangle(Vec3 a, Vec3 b, Vec3 c, int materialID)
    : Surface(materialID), a(a), b(b), c(c)
{
    this->normal = Mat::normalize(Mat::cross(b - a, c - b));
}

bool Triangle::hit(Ray ray, float startTime, float endTime, rayIntersectionInfo &record)
{
    //check the denominator first to avoid division by 0
    if (Mat::dot(ray.getDirection(), this->normal) == 0)
        return false;

    float t = Mat::dot(this->a - ray.getOrigin(), this->normal) / Mat::dot(ray.getDirection(), this->normal);

    if (t < startTime)
        return false;

    Vec3 x = ray.getOrigin() + t * ray.getDirection();

    if (Mat::dot(Mat::cross(this->b - this->a, x - this->a), this->normal) < 0)
        return false;

    if (Mat::dot(Mat::cross(this->c - this->b, x - this->b), this->normal) < 0)
        return false;

    if (Mat::dot(Mat::cross(this->a - this->c, x - this->c), this->normal) < 0)
        return false;

    if (!(startTime < t && t < endTime))
        return false;

    record.intersectionTime = t;
    record.intersectionPoint = x;
    record.surfaceNormal = this->normal;
    record.materialID = this->materialID;

    return true;

}

#endif
