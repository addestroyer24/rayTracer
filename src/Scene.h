#ifndef _SCENE_H
#define _SCENE_H

#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Surface.h"

#include <string>
#include <unordered_map>
#include <vector>

class Scene
{
private:
    //Camera camera;
    std::vector<Light*> lights;
    std::vector<Surface*> surfaces;
    std::unordered_map<std::string, Material*> materials;

public:
    Scene() = default;
    ~Scene();

    void addLight(Light*);
    void addSurface(Surface*);
    void addMaterial(Material*);

    // Camera getCamera();
    std::vector<Light*>& getLights();
    const Material* getMaterial(std::string name);

    bool hitSurface(Ray ray, float startTime, float endTime, rayIntersectionInfo &record);

};

// Scene::Scene(Camera cam)
//     : camera(cam)
// {}

Scene::~Scene()
{
    for (auto iter = lights.begin(); iter != lights.end(); iter++)
    {
        delete *iter;
    }

    for (auto iter = surfaces.begin(); iter != surfaces.end(); iter++)
    {
        delete *iter;
    }

    for (auto iter = materials.begin(); iter != materials.end(); iter++)
    {
        delete iter->second;
    }
}

void Scene::addLight(Light* light)
{
    this->lights.push_back(light);
}

void Scene::addSurface(Surface* surf)
{
    this->surfaces.push_back(surf);
}

void Scene::addMaterial(Material* mat)
{
    //I think this should work because all of the internal arrays are explicit arrays and not dynamically allocated?
    this->materials[mat->name] = mat;
}

// Camera Scene::getCamera()
// {
//     return this->camera;
// }

std::vector<Light*>& Scene::getLights()
{
    return this->lights;
}

const Material* Scene::getMaterial(std::string name)
{
    return this->materials[name];
}

bool Scene::hitSurface(Ray ray, float startTime, float endTime, rayIntersectionInfo &record)
{
    bool hitSurface = false;
    rayIntersectionInfo newInfo;

    record.intersectionTime = endTime;

    for (auto iter = this->surfaces.begin(); iter != this->surfaces.end(); iter++)
    {
        if ((*iter)->hit(ray, startTime, endTime, newInfo))
        {
            record = newInfo;
            endTime = record.intersectionTime;
            hitSurface = true;
        }
    }
    return hitSurface;
}

#endif
