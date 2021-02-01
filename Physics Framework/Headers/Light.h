#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include "stdafx.h"

struct Light
{
	v4df AmbientLight;
	v4df DiffuseLight;
	v4df SpecularLight;

	float SpecularPower;
	v3df LightVecW;
};

namespace LIGHT
{
	Light basicLight = {
		{ 0.5f, 0.5f, 0.5f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f },
		{ 0.8f, 0.8f, 0.8f, 1.0f },
		20.0f,
		{ 0.0f, 1.0f, -1.0f }
	};
}

#endif