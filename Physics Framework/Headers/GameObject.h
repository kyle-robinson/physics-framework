#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Transform.h"
#include "Appearance.h"
#include "ParticleModel.h"

using namespace DirectX;

class GameObject
{
public:
	GameObject( const std::string& type, Appearance* appearance, Transform* transform, ParticleModel* particleModel );
	std::string GetType() const { return _type; }

	Appearance* GetAppearance() const { return _appearance.get(); }
	Transform* GetTransform() const { return _transform.get(); }
	ParticleModel* GetParticleModel() const { return _particleModel.get(); }

	void Update( float deltaTime );
	void Draw( ID3D11DeviceContext* pImmediateContext );

protected:
	std::string _type;
	std::unique_ptr<Transform> _transform;
	std::unique_ptr<Appearance> _appearance;
	std::unique_ptr<ParticleModel> _particleModel;
};

#endif