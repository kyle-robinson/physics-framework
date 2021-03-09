#include "stdafx.h"
#include "RigidBody.h"
#include "Quaternion.h"

RigidBody::RigidBody()
{
	_angularDrag = 1.0f;
	_torque = { 0.0f, 0.0f, 0.0f };
	_angularVelocity = { 0.0f, 0.0f, 0.0f };
	_angularAcceleration = { 0.0f, 0.0f, 0.0f };

	ComputeBoxInertiaTensor( 1.0f, 1.0f, 1.0f );
}

void RigidBody::Update( const float dt )
{
	ComputeAngularAcceleration();
	ComputeAngularVelocity( dt );
	ApplyAngularDrag( dt );
	ComputeOrientation( dt );

	_torque = { 0.0f, 0.0f, 0.0f };
}

void RigidBody::ApplyTorque( v3df position, v3df force ) noexcept
{
	
}

void RigidBody::ApplyAngularDrag( const float dt ) noexcept
{
	_angularVelocity *= 1.0f - ( _angularDrag * dt );
}

void RigidBody::ComputeAngularAcceleration() noexcept
{
	
}

void RigidBody::ComputeAngularVelocity( const float dt ) noexcept
{
	// v = u + at
	_angularVelocity += _angularAcceleration * dt;
}

void RigidBody::ComputeOrientation( const float dt ) noexcept
{
	
}

v3df RigidBody::GetForceAtRelativePosition( v3df position ) noexcept
{
	return { 0.0f, 0.0f, 0.0f };
}

void RigidBody::ComputeBoxInertiaTensor( float sizeX, float sizeY, float sizeZ ) noexcept
{
	
}