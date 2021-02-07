#include "stdafx.h"
#include "RigidBody.h"
#include "Quaternion.h"

RigidBody::RigidBody()
{
	ParticleModel::ParticleModel();

	_angularDrag = 1.0f;
	_torque = { 0.0f, 0.0f, 0.0f };
	_angularVelocity = { 0.0f, 0.0f, 0.0f };
	_angularAcceleration = { 0.0f, 0.0f, 0.0f };

	ComputeBoxInertiaTensor( 1.0f, 1.0f, 1.0f );
}

void RigidBody::Update( const float dt ) noexcept
{
	ParticleModel::Update( dt );

	ComputeAngularAcceleration();
	ComputeAngularVelocity( dt );
	ApplyAngularDrag( dt );
	ComputeOrientation( dt );

	_torque = { 0.0f, 0.0f, 0.0f };
}

void RigidBody::ApplyTorque( v3df position, v3df force ) noexcept
{
	//_torque = force;
	//_torque = _torque.cross( position );
}

void RigidBody::ApplyAngularDrag( float angularDrag ) noexcept
{

}

void RigidBody::ComputeAngularAcceleration() noexcept
{
	
}

void RigidBody::ComputeAngularVelocity( const float dt ) noexcept
{

}

void RigidBody::ComputeOrientation( const float dt ) noexcept
{

}

void RigidBody::ComputeBoxInertiaTensor( float sizeX, float sizeY, float sizeZ ) noexcept
{

}