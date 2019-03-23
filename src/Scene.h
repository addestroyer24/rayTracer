#ifndef _SCENE_H
#define _SCENE_H

#include "BVHTree.h"
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
    std::unordered_map<std::string, Material*> materials;
    std::vector<Surface*> surfaces;

    BVHTree* sceneTree;

public:
    Scene() = default;
    ~Scene();

    void addLight(Light*);
    void addSurface(Surface*);
    void addMaterial(Material*);

    void finalizeScene();

    std::vector<Light*>& getLights();
    const Material* getMaterial(std::string name);

    bool hitSurface(Ray ray, float startTime, float endTime, rayIntersectionInfo &record);

};

Scene::~Scene()
{
    for (auto iter = lights.begin(); iter != lights.end(); iter++)
    {
        delete *iter;
    }

    for (auto iter = materials.begin(); iter != materials.end(); iter++)
    {
        delete iter->second;
    }

    delete sceneTree;
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

void Scene::finalizeScene()
{
    this->sceneTree = new BVHTree(this->surfaces);
    this->surfaces.clear();
    this->surfaces.resize(0);
}

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
    return this->sceneTree->hit(ray, startTime, endTime, record);
}

#endif
