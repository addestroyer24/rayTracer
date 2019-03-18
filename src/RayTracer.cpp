// For the test scenes, resolution of 100x100, and fov 90 degree, my
// generator creates the test images. My ray dirs are normalized.

//Hard code resolution for now
#define RES 100

#include "Camera.h"
#include "CompositeSurface.h"
#include "Light.h"
#include "Ray.h"
#include "RayGenerator.h"
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


//This might be helpful to convert from obj vectors to GenVectors
Vec3 objToGenVec(obj_vector const * objVec)
{
	Vec3 v;
	v[0] = objVec->e[0];
	v[1] = objVec->e[1];
	v[2] = objVec->e[2];
	return v;
}

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

    Vec3 position = objToGenVec(objData.vertexList[objData.camera->camera_pos_index]);
    Vec3 focus = objToGenVec(objData.vertexList[objData.camera->camera_look_point_index]);
    Vec3 up = objToGenVec(objData.normalList[objData.camera->camera_up_norm_index]);

    Camera camera = Camera::lookAt(position, focus, up, Mat::toRads(90));

	RayGenerator generator = RayGenerator(camera, RES, RES);

	//std::vector<Surface*> surfaces;
	CompositeSurface scene(-1);
	std::vector<Light> lights;

    for (int i = 0; i < objData.sphereCount; i++)
	{
		Vec3 center = objToGenVec(objData.vertexList[objData.sphereList[i]->pos_index]);
		Vec3 equator = objToGenVec(objData.normalList[objData.sphereList[i]->equator_normal_index]);
		Vec3 up = objToGenVec(objData.normalList[objData.sphereList[i]->up_normal_index]);
		float radius = Mat::magnitude(equator);

		scene.addSurface(new Sphere(center, equator, up, radius, objData.sphereList[i]->material_index));
		//surfaces.push_back(new Sphere(center, equator, up, radius, objData.sphereList[i]->material_index));
	}

	for (int i = 0; i < objData.faceCount; i++)
	{
		Vec3 a = objToGenVec(objData.vertexList[objData.faceList[i]->vertex_index[0]]);

		for (int j = 2; j < objData.faceList[i]->vertex_count; j++)
		{
			Vec3 b = objToGenVec(objData.vertexList[objData.faceList[i]->vertex_index[j - 1]]);
			Vec3 c = objToGenVec(objData.vertexList[objData.faceList[i]->vertex_index[j]]);

			scene.addSurface(new Triangle(a, b, c, objData.faceList[i]->material_index));
			//surfaces.push_back(new Triangle(a, b, c, objData.faceList[i]->material_index));
		}
	}

	for (int i = 0; i < objData.lightPointCount; i++)
	{
		Vec3 position = objToGenVec(objData.vertexList[objData.lightPointList[i]->pos_index]);
		lights.emplace_back(position, objData.lightPointList[i]->material_index);

		//scene.addSurface(new Sphere(position, Vec3(), Vec3(), 1, objData.lightPointList[i]->material_index));
	}

	float maxComponent = 1;

	for(int y=0; y<RES; y++)
	{
		for(int x=0; x<RES; x++)
		{
			bool hitSurface = false;
			rayIntersectionInfo info;
			Vec3 c{0,0,0};

			Ray r = generator.getRay(x, y);

			hitSurface = scene.hit(r, 0, 1000, info);
			
			if (hitSurface)
			{
				obj_material *surfaceMat = objData.materialList[info.materialID];

				for (auto light = lights.begin(); light < lights.end(); light++)
				{
					obj_material *lightMat = objData.materialList[(*light).getMaterial()];

					Vec3 lightDir = (*light).getPosition() - info.intersectionPoint;
					float lightDistance = Mat::magnitude(lightDir);
					lightDir = Mat::normalize(lightDir);

					//std::cout << (*light).getPosition().toString() << std::endl; 

					// diffuse lighting
					float lDotn = Mat::dot(lightDir, info.surfaceNormal);

					// specular lighting
					float spec = 0;

					if (lDotn > 0)
					{
						Vec3 reflectedLight = Mat::normalize(Mat::reflect(lightDir, info.surfaceNormal));
						Vec3 view = Mat::normalize(camera.getPosition() - info.intersectionPoint);

						if (Mat::dot(reflectedLight, view) > 0 && surfaceMat->shiny != 0)
							spec = pow(Mat::dot(reflectedLight, view), surfaceMat->shiny);
					}
					else
					{
						lDotn = 0;
					}

					Ray shadowRay(info.intersectionPoint, lightDir);
					rayIntersectionInfo unneeded;

					if (scene.hit(shadowRay, 1 - Mat::dot(lightDir, info.surfaceNormal), lightDistance, unneeded))
					{
						lDotn = 0;
						spec = 0;
					}

					for (int i = 0; i < 3; i++)
					{
						// ambient lighting
						c[i] += surfaceMat->amb[i] * lightMat->amb[i];

						// diffuse lighting
						c[i] += surfaceMat->diff[i] * lightMat->diff[i] * lDotn;

						// specular lighting
						c[i] += surfaceMat->spec[i] * lightMat->spec[i] * spec;
					}

					//c = info.surfaceNormal;
				}
			}

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

