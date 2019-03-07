#ifndef _CAMERA_H
#define _CAMERA_H

#include "libs/GenVector.h"

class Camera
{
private:
    Vector3 position;
    // Vector3 lookDir;
    // Vector3 upDir;

    Vector3 u;
    Vector3 v;
    Vector3 w;


    Camera(Vector3 position, Vector3 direction, Vector3 up);

public:
    static Camera lookAt(Vector3 position, Vector3 focus, Vector3 up);

    Vector3 getu();
    Vector3 getv();
    Vector3 getw();
    Vector3 getPosition();

};

Camera::Camera(Vector3 position, Vector3 direction, Vector3 up)
    : position(position)//, lookDir(direction), upDir(up)
{
    this->w = -direction.normalize();
    this->u = up.cross(this->w).normalize();
    this->v = this->w.cross(this->u).normalize();
}

Camera Camera::lookAt(Vector3 position, Vector3 focus, Vector3 up)
{
    return Camera(position, focus - position, up);
}

Vector3 Camera::getu()
{
    return this->u;
}

Vector3 Camera::getv()
{
    return this->v;
}

Vector3 Camera::getw()
{
    return this->w;
}

Vector3 Camera::getPosition()
{
    return this->position;
}

#endif