#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Transform.h"
#include "Appearance.h"

using namespace DirectX;

class GameObject : public Transform, public Appearance
{
public:
	GameObject( const std::string& type );
	std::string GetType() const { return _type; }
	virtual void Update( float deltaTime );
	virtual void Draw( ID3D11DeviceContext* pImmediateContext );
protected:
	std::string _type;
};

#endif