#ifndef _CAMERA_H
#define _CAMERA_H

#include "libs/Matrix.h"

class Camera
{
private:
    Vec3 position;
    // Vector3 lookDir;
    // Vector3 upDir;

    Vec3 u;
    Vec3 v;
    Vec3 w;


    Camera(Vec3 position, Vec3 direction, Vec3 up);

public:
    static Camera lookAt(Vec3 position, Vec3 focus, Vec3 up);

    Vec3 getu();
    Vec3 getv();
    Vec3 getw();
    Vec3 getPosition();

};

Camera::Camera(Vec3 position, Vec3 direction, Vec3 up)
    : position(position)//, lookDir(direction), upDir(up)
{
    this->w = normalize(-direction);
    this->u = normalize(Mat::cross(up, this->w));
    this->v = normalize(Mat::cross(this->w, this->u));
}

Camera Camera::lookAt(Vec3 position, Vec3 focus, Vec3 up)
{
    return Camera(position, focus - position, up);
}

Vec3 Camera::getu()
{
    return this->u;
}

Vec3 Camera::getv()
{
    return this->v;
}

Vec3 Camera::getw()
{
    return this->w;
}

Vec3 Camera::getPosition()
{
    return this->position;
}

#endif