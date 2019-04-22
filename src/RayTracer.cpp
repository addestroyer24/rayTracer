// For the test scenes, resolution of 100x100, and fov 90 degree, my
// generator creates the test images. My ray dirs are normalized.

//Hard code resolution for now
#define SQUARE_RES true

#if SQUARE_RES
#define RESX 5000
#define RESY 5000
#else
#define RESX 1920
#define RESY 1080
#endif

#define REFLECTION_DEPTH_LIMIT 8
#define BUNDLE_RENDER

#include "Camera.h"
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

#include <chrono>
#include <cstddef>
#include <iostream>
#include <limits>
#include <math.h> //Math functions and some constants
#include <mutex>
#include <thread>
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
void traceRayBundle(Scene& scene, rayBundle r, Vec3Bundle &vecBundle, int currentDepth = 0);

int main(int argc, char ** argv)
{
	// std::vector<Surface*> testVec;
	// testVec.push_back(new Triangle(Vec3(0), Vec3(), Vec3(), ""));
	// BVHTree tree(testVec);
	BVHNode tree;
	std::cout << "BVHNode size:\t\t" << sizeof(tree) << std::endl;
	std::cout << "subsurfaces offset:\t" << offsetof(BVHNode, surf) << std::endl;
	std::cout << "inner bool offset:\t" << offsetof(BVHNode, isInnerNode) << std::endl;
	std::cout << "boundingBox offset:\t" << offsetof(BVHNode, boundingBox) << std::endl;

	std::cout << "BVHNode alignment:\t" << alignof(tree) << std::endl;
	// std::cout << "subsurfaces alignment:\t" << alignof(tree.surf) << std::endl;
	// std::cout << "boundingBox alignment:\t" << alignof(tree.boundingBox) << std::endl;

	//exit(0);

	//Need at least two arguments (obj input and png output)
	if(argc < 3)
	{
		printf("Usage: %s input.obj output.png [-jn]\n", argv[0]);
		exit(1);
	}

	unsigned int numThreads = 1;

	if (argc == 4)
	{
		if (std::string(argv[3]).find("-j") == 0)
		{
			numThreads = std::stoi(std::string(argv[3]).substr(2));
			numThreads = std::min(std::thread::hardware_concurrency(), std::max(1U, numThreads));
		}
	}

	Buffer<Vec3> colorBuffer(RESX, RESY);

	//load obj from file argv1
    objLoader objData = objLoader();
	if (!objData.load(argv[1]))
    {
        printf("Could not load object file %s\n", argv[1]);
        exit(2);
    }

	//create a camera object
    if (!objData.camera)
    {
        printf("No camera loaded!\n");
        exit(3);
    }

    Vec3 position(objData.vertexList[objData.camera->camera_pos_index]->e);
    Vec3 focus(objData.vertexList[objData.camera->camera_look_point_index]->e);
    Vec3 up(objData.normalList[objData.camera->camera_up_norm_index]->e);

    Camera camera = Camera::lookAt(position, focus, up, Mat::toRads(90));

	RayGenerator generator = RayGenerator(camera, RESX, RESY);

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
		}
	}

	for (int i = 0; i < objData.lightPointCount; i++)
	{
		Vec3 position(objData.vertexList[objData.lightPointList[i]->pos_index]->e);

		obj_material* mat = objData.materialList[objData.lightPointList[i]->material_index];

		scene.addLight(new Light(position, mat->name));
	}

	scene.finalizeScene();

	auto startTime = std::chrono::system_clock::now();

	const int PROGRESS_BAR_SIZE = 40;
	int lastProgressPercent = -1;
	int lastProgressBarFill = 0;

	long pixelsRendered = 0;

	float maxComponent = 1;

	std::mutex compMutex;
	std::mutex progressMutex;
	//std::mutex ioMutex;

	std::cout << "\r[";
	for (int i = 0; i < PROGRESS_BAR_SIZE; i++)
	{
		std::cout << " ";
	}
	std::cout << "]  " << "0%" << std::flush;

	auto renderFunc = [&](int offset){
		float localMaxComponent = 1;
		int localPixelsRendered = 0;
#ifndef BUNDLE_RENDER
		for (int y = 0; y < RESY; y++)
		{
			for (int x = offset; x<RESX; x += numThreads)
			{
				Ray r = generator.getRay(x, y);

				Vec3 c = traceRay(scene, r);

				for (int i = 0; i < 3; i++)
				{
					if (c[i] > localMaxComponent)
						localMaxComponent = c[i];
				}

				colorBuffer.at(x, RESY - 1 - y) = c;

				if (localPixelsRendered > RESX * RESY / 500 && (numThreads == 1 || progressMutex.try_lock()))
				{
					pixelsRendered += localPixelsRendered + 1;
#else
		for (int y = offset * 2; y < RESY; y += 2*numThreads)
		{
			for (int x = 0; x<RESX; x += 2)
			{
				rayBundle rayBundle;
				generator.getRayBundle(x, y, rayBundle);

				Vec3Bundle vecBundle;
				traceRayBundle(scene, rayBundle, vecBundle);

				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						colorBuffer.at(x + (j%2), RESY - 1 - y - (j/2))[i] = vecBundle[j][i];

						if (vecBundle[j][i] > localMaxComponent)
							localMaxComponent = vecBundle[j][i];
					}
				}

				if (localPixelsRendered > RESX * RESY / 500 && (numThreads == 1 || progressMutex.try_lock()))
				{
					pixelsRendered += localPixelsRendered + 4;
#endif
					localPixelsRendered = 0;

					int progressPercent = pixelsRendered * 100 / (RESX * RESY);
					int progressBarFill = pixelsRendered * PROGRESS_BAR_SIZE / (RESX * RESY);

					if (progressPercent != lastProgressPercent || progressBarFill != lastProgressBarFill)
					{
						lastProgressPercent = progressPercent;
						std::cout << "\r[";
						for (int i = 0; i < PROGRESS_BAR_SIZE; i++)
						{
							std::cout << (i < progressBarFill ? "#" : " ");
						}
						std::cout << "]  " << progressPercent << "%" << std::flush;
					}
					if (numThreads != 1) progressMutex.unlock();
				}
				else
				{
#ifndef BUNDLE_RENDER
					localPixelsRendered++;
#else
					localPixelsRendered += 4;
#endif
				}
			}
		}

		{
			std::lock_guard<std::mutex> lk(compMutex);
			maxComponent = std::max(maxComponent, localMaxComponent);
		}
	};

	std::vector<std::thread> threads;

	for (int i = 1; i < numThreads; i++)
	{
		threads.emplace_back(renderFunc, i);
	}

	renderFunc(0);

	for (auto &thread : threads)
	{
		thread.join();
	}

	threads.clear();

	std::cout << "\r[";
	for (int i = 0; i < PROGRESS_BAR_SIZE; i++)
	{
		std::cout << "#";
	}
	std::cout << "]  " << "100%" << std::endl;

	std::cout << std::chrono::duration<double>(std::chrono::system_clock::now() - startTime).count() << std::endl;

	//create a frame buffer for RESxRES
    Buffer<Color> outputBuffer(RESX, RESY);

	auto convertFunc = [&outputBuffer, &colorBuffer, maxComponent, numThreads](int offset)
	{
		for (int y = 0; y < RESY; y++)
		{
			for (int x = offset; x < RESX; x += numThreads)
			{
				outputBuffer.at(x, y) = static_cast<Color>(colorBuffer.at(x, y) * 255 / maxComponent);
			}
		}
	};

	for (int i = 1; i < numThreads; i++)
	{
		threads.emplace_back(convertFunc, i);
	}

	convertFunc(0);

	for (auto &thread : threads)
	{
		thread.join();
	}

	//Write output buffer to file argv2
	simplePNG_write(argv[2], outputBuffer.getWidth(), outputBuffer.getHeight(), (unsigned char*)&outputBuffer.at(0,0));

	return 0;
}


Vec3 traceRay(Scene& scene, Ray r, int currentDepth)
{
	rayHit surfaceInfo;
	Vec3 returnColor(0);
	
	if (!scene.hitSurface(r, 0, 1000000, surfaceInfo))
		return returnColor;
	
	if (surfaceInfo.materialID == "")
		return surfaceInfo.surfaceNormal / 2 + 0.5;

	const Material* surfaceMat = scene.getMaterial(surfaceInfo.materialID);

	for (auto* light : scene.getLights())
	{
		const Material* lightMat = scene.getMaterial(light->getMaterialName());

		Vec3 lightDir = light->getPosition() - surfaceInfo.intersectionPoint;
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
		rayHit unneeded;

		if (scene.hitSurface(shadowRay, 0, lightDistance, unneeded))
		{
			lDotn = 0;
			spec = 0;
		}

		Vec3 surfaceColor(0);

		surfaceColor += surfaceMat->amb * lightMat->amb;

		surfaceColor += surfaceMat->diff * lightMat->diff * lDotn;

		surfaceColor += surfaceMat->spec * surfaceMat->spec * spec;

		returnColor += surfaceColor;
	}

	if (surfaceMat->reflect > 0 && currentDepth <= REFLECTION_DEPTH_LIMIT)
	{
		Ray reflectedRay(surfaceInfo.intersectionPoint + surfaceInfo.surfaceNormal * 0.0001f, 
			Mat::reflectIn(r.getDirection(), surfaceInfo.surfaceNormal));

		Vec3 reflectColor = traceRay(scene, reflectedRay, currentDepth + 1);

		returnColor = returnColor * (1 - surfaceMat->reflect) + reflectColor * surfaceMat->reflect;
	}

	return returnColor;
}

void traceRayBundle(Scene& scene, rayBundle rays, Vec3Bundle &vecBundle, int currentDepth)
{
	const float DEFAULT_END_TIME = 1000000;

	hitBundle records;

	for (int i = 0; i < 4; i++) records[i].intersectionTime = DEFAULT_END_TIME;

	scene.hitSurface(rays, 0, DEFAULT_END_TIME, records);

	for (int i = 0; i < 4; i++)
	{
		Vec3 returnColor(0);

		if (records[i].intersectionTime == DEFAULT_END_TIME)
		{
			vecBundle[i] = returnColor;
			continue;
		}

		if (records[i].materialID == "")
			vecBundle[i] = records[i].surfaceNormal / 2 + 0.5;

		const Material* surfaceMat = scene.getMaterial(records[i].materialID);

		for (auto* light : scene.getLights())
		{
			const Material* lightMat = scene.getMaterial(light->getMaterialName());

			Vec3 lightDir = light->getPosition() - records[i].intersectionPoint;
			float lightDistance = Mat::magnitude(lightDir);
			lightDir = Mat::normalize(lightDir);

			//std::cout << (*light).getPosition().toString() << std::endl; 

			// diffuse lighting
			float lDotn = Mat::dot(lightDir, records[i].surfaceNormal);

			// specular lighting
			float spec = 0;

			if (lDotn > 0)
			{
				Vec3 reflectedLight = Mat::normalize(Mat::reflectOut(lightDir, records[i].surfaceNormal));
				Vec3 view = Mat::normalize(-rays[i].getDirection());
				// Vec3 view = Mat::normalize(scene.getCamera().getPosition() - surfaceInfo.intersectionPoint);

				if (Mat::dot(reflectedLight, view) > 0 && surfaceMat->shiny != 0)
					spec = pow(Mat::dot(reflectedLight, view), surfaceMat->shiny);
			}
			else
			{
				lDotn = 0;
			}

			Ray shadowRay(records[i].intersectionPoint + records[i].surfaceNormal * 0.0001f, lightDir);
			rayHit unneeded;

			if (scene.hitSurface(shadowRay, 0, lightDistance, unneeded))
			{
				lDotn = 0;
				spec = 0;
			}

			Vec3 surfaceColor(0);

			surfaceColor += surfaceMat->amb * lightMat->amb;

			surfaceColor += surfaceMat->diff * lightMat->diff * lDotn;

			surfaceColor += surfaceMat->spec * surfaceMat->spec * spec;

			returnColor += surfaceColor;
		}

		if (surfaceMat->reflect > 0 && currentDepth <= REFLECTION_DEPTH_LIMIT)
		{
			Ray reflectedRay(records[i].intersectionPoint + records[i].surfaceNormal * 0.0001f, 
				Mat::reflectIn(rays[i].getDirection(), records[i].surfaceNormal));

			Vec3 reflectColor = traceRay(scene, reflectedRay, currentDepth + 1);

			returnColor = returnColor * (1 - surfaceMat->reflect) + reflectColor * surfaceMat->reflect;
		}

		vecBundle[i] = returnColor;
	}
}
