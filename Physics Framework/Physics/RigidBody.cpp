#include "stdafx.h"
#include "RigidBody.h"
#include "Quaternion.h"

RigidBody::RigidBody()
{
	ParticleModel::ParticleModel();

	_angularDrag = 1.0f;
	_torque = XMVectorZero();
	_angularVelocity = XMVectorZero();
	_angularAcceleration = XMVectorZero();

	ComputeBoxInertiaTensor( 1.0f, 1.0f, 1.0f );
}

void RigidBody::Update( const float dt )
{
	ParticleModel::Update( dt );

	ComputeAngularAcceleration();
	ComputeAngularVelocity( dt );
	ApplyAngularDrag( dt );
	ComputeOrientation( dt );

	_torque = XMVectorZero();
}

void RigidBody::ApplyTorque( XMVECTOR position, XMVECTOR force ) noexcept
{
	XMMATRIX inverseRotation = XMMatrixInverse( nullptr, GetTransform()->GetRotationMatrix() );
	float magnitude = XMVectorGetX( XMVector3Length( force ) ) / XMVectorGetX( XMVector3Length( position ) );
	_torque += XMVector3Normalize( XMVector3Transform( XMVector3Cross( XMVector3Normalize( position ),
		XMVector3Normalize( force ) ), inverseRotation ) ) * magnitude;
}

void RigidBody::ApplyAngularDrag( const float dt ) noexcept
{
	_angularVelocity *= 1.0f - ( _angularDrag * dt );
}

void RigidBody::ComputeAngularAcceleration() noexcept
{
	_angularAcceleration = XMVector3Transform( _torque, _inverseInertiaTensor );
}

void RigidBody::ComputeAngularVelocity( const float dt ) noexcept
{
	// v = u + at
	_angularVelocity += _angularAcceleration * dt;
}

void RigidBody::ComputeOrientation( const float dt ) noexcept
{
	XMVECTOR orientationChange = _angularVelocity * dt;
	XMVECTOR oldOrientation = GetTransform()->GetOrientation();
	XMVECTOR newOrientation = XMQuaternionNormalize( XMQuaternionMultiply( orientationChange, oldOrientation ) );

	GetTransform()->SetOrientationQuaternion( newOrientation );
}

XMVECTOR RigidBody::GetForceAtRelativePosition( XMVECTOR position ) noexcept
{
	XMMATRIX inverseRotation = XMMatrixInverse( nullptr, GetTransform()->GetRotationMatrix() );
	XMVECTOR relativePosition = XMVector3Transform( position, inverseRotation );
	
	float magnitude = XMVectorGetX( XMVector3Length( relativePosition ) ) * XMVectorGetX( XMVector3Length( _angularVelocity ) );
	XMVECTOR forceLocal = XMVector3Normalize( XMVector3Cross( XMVector3Transform( _angularVelocity, _inertiaTensor ),
		relativePosition ) ) * magnitude;
	
	v3df momentum = GetVelocity() * GetMass();
	XMVECTOR actingForce = XMVector3Transform( forceLocal, GetTransform()->GetRotationMatrix() );
	actingForce = XMVectorSet( XMVectorGetX( actingForce ) + momentum.x, XMVectorGetY( actingForce ) + momentum.y,
		XMVectorGetZ( actingForce ) + momentum.z, XMVectorGetW( actingForce ) );

	return actingForce;

}

void RigidBody::ComputeBoxInertiaTensor( float sizeX, float sizeY, float sizeZ ) noexcept
{
	_inertiaTensor = XMMatrixIdentity();

	float sizeX_squared = sizeX * sizeX;
	float sizeY_squared = sizeY * sizeY;
	float sizeZ_squared = sizeZ * sizeZ;

	_inertiaTensor.r[0].m128_f32[0] = 0.83333f * GetMass() * ( sizeY_squared + sizeZ_squared );
	_inertiaTensor.r[1].m128_f32[1] = 0.83333f * GetMass() * ( sizeX_squared + sizeZ_squared );
	_inertiaTensor.r[2].m128_f32[2] = 0.83333f * GetMass() * ( sizeX_squared + sizeY_squared );

	XMVECTOR determinant = XMMatrixDeterminant( _inertiaTensor );
	_inverseInertiaTensor = XMMatrixInverse( &determinant, _inertiaTensor );
}