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

    float fov;


    Camera(Vec3 position, Vec3 direction, Vec3 up, float fov);

public:
    static Camera lookAt(Vec3 position, Vec3 focus, Vec3 up, float fov);

    Vec3 getu() const;
    Vec3 getv() const;
    Vec3 getw() const;
    Vec3 getPosition() const;
    float getFOV() const;

};

Camera::Camera(Vec3 position, Vec3 direction, Vec3 up, float fov)
    : position(position), fov(fov)//, lookDir(direction), upDir(up)
{
    this->w = normalize(-direction);
    this->u = normalize(Mat::cross(up, this->w));
    this->v = normalize(Mat::cross(this->w, this->u));
}

Camera Camera::lookAt(Vec3 position, Vec3 focus, Vec3 up, float fov)
{
    return Camera(position, focus - position, up, fov);
}

Vec3 Camera::getu() const
{
    return this->u;
}

Vec3 Camera::getv() const
{
    return this->v;
}

Vec3 Camera::getw() const
{
    return this->w;
}

Vec3 Camera::getPosition() const
{
    return this->position;
}

float Camera::getFOV() const
{
    return this->fov;
}

#endif