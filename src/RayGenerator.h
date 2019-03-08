#ifndef _RAY_GENERATOR_H
#define _RAY_GENERATOR_H

#include "Camera.h"
#include "Ray.h"

#include <cmath>

class RayGenerator
{
private:
    Camera camera;
    int imageWidth;
    int imageHeight;
    float fromCameraToLeft;
    float fromCameraToRight;
    float fromCameraToTop;
    float fromCameraToBottom;

public:
    RayGenerator(Camera camera, int xDim, int yDim);

    Ray getRay(int x, int y);

};

RayGenerator::RayGenerator(Camera camera, int xDim, int yDim)
    : camera(camera), imageWidth(xDim), imageHeight(yDim)
{
    this->fromCameraToLeft = -this->imageWidth / 2.0f;
    this->fromCameraToRight = this->imageWidth / 2.0f;
    this->fromCameraToTop = this->imageHeight / 2.0f;
    this->fromCameraToBottom = -this->imageHeight / 2.0f;
}

Ray RayGenerator::getRay(int x, int y)
{
    float u = this->fromCameraToLeft + (this->fromCameraToRight - this->fromCameraToLeft) * (x + 0.5f) / this->imageWidth;
    float v = this->fromCameraToBottom + (this->fromCameraToTop - this->fromCameraToBottom) * (y + 0.5f) / this->imageHeight;
    float d = this->imageWidth / 2.0f / tanf(M_PI_4);

    Vec3 direction = u * this->camera.getu() + v * this->camera.getv() - d * this->camera.getw();

    // s = direction + e
    // direction = s - e

    return Ray(this->camera.getPosition(), Mat::normalize(direction));
}

#endif
