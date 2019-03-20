// For the test scenes, resolution of 100x100, and fov 90 degree, my
// generator creates the test images. My ray dirs are normalized.

//Hard code resolution for now
#define RES 100

#define REFLECTION_DEPTH_LIMIT 5

#include "Camera.h"
//#include "CompositeSurface.h"
#include "Light.h"
#include "Material.h"
#include "Ray.h"
#include "RayGenerator.h"
#include "Scene.h"
#include "Sphere.h"
#include "Surface.h"
#include "Triangle.h"

#include "libs/Buffer.h"
#include "libs/Matrix.h"
#include "libs/objLoader.h"
#include "libs/simplePNG.h"

#define _USE_MATH_DEFINES //This enables math constants in Windows
#include <iostream>
#include <limits>
#include <math.h> //Math functions and some constants
#include <vector>


Material* objMaterialtoMaterial(obj_material* mat)
{
	return new Material(
		mat->name,
		mat->texture_filename,
		Vec3(mat->amb),
		Vec3(mat->diff),
		Vec3(mat->spec),
		mat->reflect,
		mat->refract,
		mat->trans,
		mat->shiny,
		mat->glossy,
		mat->refract_index
	);
}


Vec3 traceRay(Scene& scene, Ray r, int currentDepth = 0);

int main(int argc, char ** argv)
{
	//Need at least two arguments (obj input and png output)
	if(argc < 3)
	{
		printf("Usage: %s input.obj output.png\n", argv[0]);
		exit(0);
	}

	Buffer<Vec3> colorBuffer(RES, RES);

	//load obj from file argv1
    objLoader objData = objLoader();
	if (!objData.load(argv[1]))
    {
        printf("Could not load object file %s\n", argv[1]);
        exit(0);
    }

	//create a camera object
    if (!objData.camera)
    {
        printf("No camera loaded!\n");
        exit(0);
    }

    Vec3 position(objData.vertexList[objData.camera->camera_pos_index]->e);
    Vec3 focus(objData.vertexList[objData.camera->camera_look_point_index]->e);
    Vec3 up(objData.normalList[objData.camera->camera_up_norm_index]->e);

    Camera camera = Camera::lookAt(position, focus, up, Mat::toRads(90));

	RayGenerator generator = RayGenerator(camera, RES, RES);

	//std::vector<Surface*> surfaces;
	//CompositeSurface scene(-1);
	Scene scene;
	std::vector<Light> lights;

	for (int i = 0; i < objData.materialCount; i++)
	{
		scene.addMaterial(objMaterialtoMaterial(objData.materialList[i]));
	}

    for (int i = 0; i < objData.sphereCount; i++)
	{
		Vec3 center(objData.vertexList[objData.sphereList[i]->pos_index]->e);
		Vec3 equator(objData.normalList[objData.sphereList[i]->equator_normal_index]->e);
		Vec3 up(objData.normalList[objData.sphereList[i]->up_normal_index]->e);
		float radius = Mat::magnitude(equator);

		obj_material* mat = objData.materialList[objData.sphereList[i]->material_index];

		scene.addSurface(new Sphere(center, equator, up, radius, mat->name));
		//surfaces.push_back(new Sphere(center, equator, up, radius, objData.sphereList[i]->material_index));
	}

	for (int i = 0; i < objData.faceCount; i++)
	{
		Vec3 a(objData.vertexList[objData.faceList[i]->vertex_index[0]]->e);

		for (int j = 2; j < objData.faceList[i]->vertex_count; j++)
		{
			Vec3 b(objData.vertexList[objData.faceList[i]->vertex_index[j - 1]]->e);
			Vec3 c(objData.vertexList[objData.faceList[i]->vertex_index[j]]->e);

			obj_material* mat = objData.materialList[objData.faceList[i]->material_index];

			scene.addSurface(new Triangle(a, b, c, mat->name));
			//surfaces.push_back(new Triangle(a, b, c, objData.faceList[i]->material_index));
		}
	}

	for (int i = 0; i < objData.lightPointCount; i++)
	{
		Vec3 position(objData.vertexList[objData.lightPointList[i]->pos_index]->e);

		obj_material* mat = objData.materialList[objData.lightPointList[i]->material_index];

		scene.addLight(new Light(position, mat->name));
		//lights.emplace_back(position, mat->name);

		//scene.addSurface(new Sphere(position, Vec3(), Vec3(), 1, objData.lightPointList[i]->material_index));
	}

	float maxComponent = 1;

	for(int y=0; y<RES; y++)
	{
		for(int x=0; x<RES; x++)
		{
			Ray r = generator.getRay(x, y);

			Vec3 c = traceRay(scene, r);

			for (int i = 0; i < 3; i++)
			{
				if (c[i] > maxComponent)
					maxComponent = c[i];
			}

			colorBuffer.at(x,RES - 1 - y) = c;
		}
	}

	// for (auto iter = surfaces.begin(); iter != surfaces.end(); iter++)
	// {
	// 	delete *iter;
	// }
	// surfaces.clear();

	//create a frame buffer for RESxRES
    Buffer<Color> outputBuffer(RES, RES);

	for(int y=0; y<RES; y++)
	{
		for(int x=0; x<RES; x++)
		{
			Vec3& floatColor = colorBuffer.at(x, y);
			Color& charColor = outputBuffer.at(x, y);
			charColor = (Color)(floatColor * 255 / maxComponent);
			// charColor[0] = (unsigned char)(floatColor[0] * 255 / maxComponent);
			// charColor[1] = (unsigned char)(floatColor[1] * 255 / maxComponent);
			// charColor[2] = (unsigned char)(floatColor[2] * 255 / maxComponent);
		}
	}

	//Write output buffer to file argv2
	simplePNG_write(argv[2], outputBuffer.getWidth(), outputBuffer.getHeight(), (unsigned char*)&outputBuffer.at(0,0));

	return 0;
}


Vec3 traceRay(Scene& scene, Ray r, int currentDepth)
{
	bool hitSurface = false;
	rayIntersectionInfo surfaceInfo;
	Vec3 returnColor(0);

	hitSurface = scene.hitSurface(r, 0, 1000, surfaceInfo);
	
	if (!hitSurface)
		return returnColor;

	const Material* surfaceMat = scene.getMaterial(surfaceInfo.materialID);

	for (auto light = scene.getLights().begin(); light < scene.getLights().end(); light++)
	{
		const Material* lightMat = scene.getMaterial((*light)->getMaterialName());

		Vec3 lightDir = (*light)->getPosition() - surfaceInfo.intersectionPoint;
		float lightDistance = Mat::magnitude(lightDir);
		lightDir = Mat::normalize(lightDir);

		//std::cout << (*light).getPosition().toString() << std::endl; 

		// diffuse lighting
		float lDotn = Mat::dot(lightDir, surfaceInfo.surfaceNormal);

		// specular lighting
		float spec = 0;

		if (lDotn > 0)
		{
			Vec3 reflectedLight = Mat::normalize(Mat::reflectOut(lightDir, surfaceInfo.surfaceNormal));
			Vec3 view = Mat::normalize(-r.getDirection());
			// Vec3 view = Mat::normalize(scene.getCamera().getPosition() - surfaceInfo.intersectionPoint);

			if (Mat::dot(reflectedLight, view) > 0 && surfaceMat->shiny != 0)
				spec = pow(Mat::dot(reflectedLight, view), surfaceMat->shiny);
		}
		else
		{
			lDotn = 0;
		}

		Ray shadowRay(surfaceInfo.intersectionPoint + surfaceInfo.surfaceNormal * 0.0001f, lightDir);
		rayIntersectionInfo unneeded;

		if (scene.hitSurface(shadowRay, 0, lightDistance, unneeded))
		{
			lDotn = 0;
			spec = 0;
		}

		Vec3 surfaceColor(0);

		surfaceColor += surfaceMat->amb * lightMat->amb;

		surfaceColor += surfaceMat->diff * lightMat->diff * lDotn;

		surfaceColor += surfaceMat->spec * surfaceMat->spec * spec;

		if (surfaceMat->reflect > 0 && currentDepth <= REFLECTION_DEPTH_LIMIT)
		{
			Ray reflectedRay(surfaceInfo.intersectionPoint + surfaceInfo.surfaceNormal * 0.0001f, 
				Mat::reflectIn(r.getDirection(), surfaceInfo.surfaceNormal));

			Vec3 reflectColor = traceRay(scene, reflectedRay, currentDepth + 1);

			surfaceColor = surfaceColor * (1 - surfaceMat->reflect) + reflectColor * surfaceMat->reflect;
		}

		returnColor += surfaceColor;
	}

	//c = surfaceInfo.surfaceNormal;

	return returnColor;
}
