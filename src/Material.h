#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "libs/Matrix.h"

#include <string>

struct Material
{
public:
    std::string name;
	std::string texture_filename;
	Vec3 amb;
	Vec3 diff;
	Vec3 spec;
	double reflect;
	double refract;
	double trans;
	double shiny;
	double glossy;
	double refract_index;
};

#endif
