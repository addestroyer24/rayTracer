// For the test scenes, resolution of 100x100, and fov 90 degree, my
// generator creates the test images. My ray dirs are normalized.

//Hard code resolution for now
#define RES 100


#define _USE_MATH_DEFINES //This enables math constants in Windows
#include <math.h> //Math functions and some constants

//TODO: add any other includes you need; possibilities:
#include "RayGenerator.h"
#include "Ray.h"
#include "Camera.h"
#include "libs/objLoader.h"
#include "libs/GenVector.h"
#include "libs/Buffer.h"
#include "libs/simplePNG.h"


//This might be helpful to convert from obj vectors to GenVectors
Vector3 objToGenVec(obj_vector const * objVec)
{
	Vector3 v;
	v[0] = objVec->e[0];
	v[1] = objVec->e[1];
	v[2] = objVec->e[2];
	return v;
}

int main(int argc, char ** argv)
{

	//TODO: create a frame buffer for RESxRES
    Buffer<Color> buffer = Buffer<Color>(RES, RES);

	//Need at least two arguments (obj input and png output)
	if(argc < 3)
	{
		printf("Usage: %s input.obj output.png\n", argv[0]);
		exit(0);
	}

	//TODO: load obj from file argv1
    objLoader objData = objLoader();
	objData.load(argv[1]);

	//TODO: create a camera object
    if (!objData.camera)
    {
        printf("No camera loaded!\n");
        exit(0);
    }

    Vector3 position = objToGenVec(objData.vertexList[objData.camera->camera_pos_index]);
    Vector3 focus = objToGenVec(objData.vertexList[objData.camera->camera_look_point_index]);
    Vector3 up = objToGenVec(objData.normalList[objData.camera->camera_up_norm_index]);

    Camera camera = Camera::lookAt(position, focus, up);

	RayGenerator generator = RayGenerator(camera, RES, RES);

	//Convert vectors to RGB colors for testing results
	for(int y=0; y<RES; y++)
	{
		for(int x=0; x<RES; x++)
		{
			Ray r = generator.getRay(x, y);
			Vector3 d = r.getDirection()*255.0f;
			Color c = Color( fabs(d[0]), fabs(d[1]), fabs(d[2]) );
            //TODO ask Micah
			buffer.at(x,RES - 1 - y) = c;
		}
	}

	//Write output buffer to file argv2
	simplePNG_write(argv[2], buffer.getWidth(), buffer.getHeight(), (unsigned char*)&buffer.at(0,0));

	return 0;
}

