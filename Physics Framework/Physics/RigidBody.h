#pragma once
#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "Appearance.h"
#include "ParticleModel.h"

class RigidBody : public ParticleModel
{
public:
	RigidBody();
	void Update( const float dt ) override;

	// apply forces
	void ApplyTorque( XMVECTOR position, XMVECTOR force ) noexcept;
	void ApplyAngularDrag( const float dt ) noexcept;

	// compute forces
	void ComputeAngularAcceleration() noexcept;
	void ComputeAngularVelocity( const float dt ) noexcept;
	void ComputeOrientation( const float dt ) noexcept;

	XMVECTOR GetForceAtRelativePosition( XMVECTOR position ) noexcept;

	// inertia tensors
	void ComputeBoxInertiaTensor( float sizeX, float sizeY, float sizeZ ) noexcept;
private:
	float _angularDrag;

	XMVECTOR _torque;
	XMVECTOR _angularVelocity;
	XMVECTOR _angularAcceleration;

	XMMATRIX _inertiaTensor;
	XMMATRIX _inverseInertiaTensor;
};

#endif