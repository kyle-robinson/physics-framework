#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "../Model/Transform.h"
#include "../Model/Appearance.h"

using namespace DirectX;

class GameObject : public Transform, public Appearance
{
public:
	GameObject( std::string type, Geometry geometry, Material material );
	std::string GetType() const { return _type; }

	void Update( float t );
	void Draw( ID3D11DeviceContext* pImmediateContext );

private:
	std::string _type;
};

#endif