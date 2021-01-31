#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Appearance.h"
#include "ParticleModel.h"

using namespace DirectX;

class GameObject
{
public:
	GameObject( const std::string& type );

	std::string GetType() const { return _type; }
	virtual void Update();
	virtual void Draw( ID3D11DeviceContext* pImmediateContext );

	std::shared_ptr<Transform> GetTransform() const { return _transform; }
	std::shared_ptr<Appearance> GetAppearance() const { return _appearance; }
	std::shared_ptr<ParticleModel> GetParticleModel() const { return _particleModel; }
protected:
	std::string _type;
	std::shared_ptr<Transform> _transform;
	std::shared_ptr<Appearance> _appearance;
	std::shared_ptr<ParticleModel> _particleModel;
};

#endif