#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Appearance.h"
#include "ParticleModel.h"

using namespace DirectX;

class GameObject
{
public:
	GameObject( const std::string& id );

	virtual void Update();
	virtual void Draw( ID3D11DeviceContext* pImmediateContext );
	const std::string& GetID() const noexcept { return _id; }

	std::shared_ptr<Transform> GetTransform() const noexcept { return _transform; }
	std::shared_ptr<Appearance> GetAppearance() const noexcept { return _appearance; }
	std::shared_ptr<ParticleModel> GetParticleModel() const noexcept { return _particleModel; }
	//std::shared_ptr<ParticleSystem> GetParticleSystem() const noexcept { return _particleSystem; };
protected:
	std::string _id;
	std::shared_ptr<Transform> _transform;
	std::shared_ptr<Appearance> _appearance;
	std::shared_ptr<ParticleModel> _particleModel;
	//std::shared_ptr<ParticleSystem> _particleSystem;
};

#endif