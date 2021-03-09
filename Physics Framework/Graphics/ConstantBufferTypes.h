#pragma once
#ifndef CONSTANTBUFFERTYPES_H
#define CONSTANTBUFFERTYPES_H

#include <DirectXMath.h>
#include "Structures.h"

struct CB_VS_matrix
{
	DirectX::XMMATRIX World;
	DirectX::XMMATRIX View;
	DirectX::XMMATRIX Projection;
	SurfaceInfo surface;
	Light light;
	v3df EyePosW;
	float HasTexture;
	float UseLighting;
};

#endif