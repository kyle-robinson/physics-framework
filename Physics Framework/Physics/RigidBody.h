#pragma once
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "ParticleModel.h"

class RigidBody : public ParticleModel
{
public:
	RigidBody();
	void Update( const float dt ) override;

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

	XMFLOAT4X4 _inertiaTensor;
	XMFLOAT4X4 _inverseInertiaTensor;
};

#endif