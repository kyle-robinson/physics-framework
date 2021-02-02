#pragma once
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "Vector.h"

struct Light
{
	v4df AmbientLight;
	v4df DiffuseLight;
	v4df SpecularLight;

	float SpecularPower;
	v3df LightVecW;
};

struct SurfaceInfo
{
	v4df AmbientMtrl;
	v4df DiffuseMtrl;
	v4df SpecularMtrl;
};

#endif