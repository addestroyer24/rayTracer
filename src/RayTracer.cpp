#include "libs/GenVector.h"
#include "libs/simplePNG.h"
#include "libs/Buffer.h"
#include "libs/objLoader.h"

int main(int argc, char ** argv)
{
	//red color
	Color c = Color(255, 0, 0);
	printf("red is: %i %i %i\n", c[0], c[1], c[2]);

	//buffer
	Buffer<Color> b = Buffer<Color>(40, 40);
	b.at(4,4) = c; //set pixel at 4,4 to red

	//save buffer as png image
	simplePNG_write("test.png", b.getWidth(), b.getHeight(), (unsigned char*)&b.at(0,0));

	//need at least one argument (obj file)
	if(argc < 2)
	{
		printf("Usage %s filename.obj\n", argv[0]);
		exit(0);
	}

	//load camera data
	objLoader objData = objLoader();
	objData.load(argv[1]);

	if(objData.camera != NULL)
	{
		printf("Found a camera\n");
		printf(" position: ");
		printf("%f %f %f\n",
				objData.vertexList[ objData.camera->camera_pos_index ]->e[0],
				objData.vertexList[ objData.camera->camera_pos_index ]->e[1],
				objData.vertexList[ objData.camera->camera_pos_index ]->e[2]
				);
		printf(" looking at: ");
		printf("%f %f %f\n",
				objData.vertexList[ objData.camera->camera_look_point_index ]->e[0],
				objData.vertexList[ objData.camera->camera_look_point_index ]->e[1],
				objData.vertexList[ objData.camera->camera_look_point_index ]->e[2]
				);
		printf(" up normal: ");
		printf("%f %f %f\n",
				objData.normalList[ objData.camera->camera_up_norm_index ]->e[0],
				objData.normalList[ objData.camera->camera_up_norm_index ]->e[1],
				objData.normalList[ objData.camera->camera_up_norm_index ]->e[2]
				);
	}

	return 0;
}
