#pragma once
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Transform.h"
#include "ParticleModel.h"

class RigidBody : public ParticleModel
{
public:
	RigidBody();
	void Update( const float dt ) override;
	std::shared_ptr<Transform> GetTransform() const noexcept { return _transform; };

	// apply forces
	void ApplyTorque( v3df position, v3df force ) noexcept;
	void ApplyAngularDrag( const float dt ) noexcept;

	// compute forces
	void ComputeAngularAcceleration() noexcept;
	void ComputeAngularVelocity( const float dt ) noexcept;
	void ComputeOrientation( const float dt ) noexcept;

	v3df GetForceAtRelativePosition( v3df position ) noexcept;

	// inertia tensors
	void ComputeBoxInertiaTensor( float sizeX, float sizeY, float sizeZ ) noexcept;
private:
	float _angularDrag;

	v3df _torque;
	v3df _angularVelocity;
	v3df _angularAcceleration;

	XMMATRIX _inertiaTensor;
	XMMATRIX _inverseInertiaTensor;

	std::shared_ptr<Transform> _transform;
};

#endif