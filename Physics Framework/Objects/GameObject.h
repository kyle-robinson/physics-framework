#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "RigidBody.h"
#include "Appearance.h"

using namespace DirectX;

class GameObject
{
public:
	GameObject( const std::string& id );

	void UpdateTransforms();
	virtual void Update( const float dt );
	virtual void Draw( ID3D11DeviceContext* pImmediateContext );
	const std::string& GetID() const noexcept { return _id; }

	void SetPosition( float x, float y, float z ) noexcept
	{
		_transform->SetPosition( x, y, z );
		_rigidBody->SetPosition( x, y, z );
		_rigidBody->CalculateDerivedData();
	}

	void SetRotation( float x, float y, float z ) noexcept
	{
		_transform->SetRotation( x, y, z );
		_rigidBody->SetRotation( x, y, z );
		_rigidBody->CalculateDerivedData();
	}

	std::shared_ptr<RigidBody> GetRigidBody() const noexcept { return _rigidBody; };
	std::shared_ptr<Transform> GetTransform() const noexcept { return _transform; }
	std::shared_ptr<Appearance> GetAppearance() const noexcept { return _appearance; }
	std::shared_ptr<ParticleModel> GetParticleModel() const noexcept { return _particleModel; }
protected:
	std::string _id;
	std::shared_ptr<RigidBody> _rigidBody;
	std::shared_ptr<Transform> _transform;
	std::shared_ptr<Appearance> _appearance;
	std::shared_ptr<ParticleModel> _particleModel;
};

#endif