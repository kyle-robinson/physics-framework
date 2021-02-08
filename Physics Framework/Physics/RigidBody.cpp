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

void RigidBody::Update( const float dt )
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
	XMMATRIX inverseRotation = XMMatrixInverse( nullptr, GetTransform()->GetRotationMatrix() );
	XMVECTOR forceVec = XMLoadFloat3( &XMFLOAT3( force[0], force[1], force[2] ) );
	XMVECTOR positionVec = XMLoadFloat3( &XMFLOAT3( position[0], position[1], position[2] ) );
	float magnitude = XMVectorGetX( XMVector3Length( forceVec ) ) / XMVectorGetX( XMVector3Length( positionVec ) );
	XMVECTOR netTorque = XMVector3Normalize( XMVector3Transform( XMVector3Cross( XMVector3Normalize( positionVec ), XMVector3Normalize( forceVec ) ), inverseRotation ) ) * magnitude;
	_torque += { XMVectorGetX( netTorque ), XMVectorGetY( netTorque ), XMVectorGetZ( netTorque ) };
}

void RigidBody::ApplyAngularDrag( const float dt ) noexcept
{
	_angularVelocity *= 1.0f - ( _angularDrag * dt );
}

void RigidBody::ComputeAngularAcceleration() noexcept
{
	XMVECTOR netTorque = XMLoadFloat3( &XMFLOAT3( _torque[0], _torque[1], _torque[2] ) );
	XMVECTOR accel = XMVector3Transform( netTorque, _inverseInertiaTensor );
	_angularAcceleration = { XMVectorGetX( accel ), XMVectorGetY( accel ), XMVectorGetZ( accel ) };
}

void RigidBody::ComputeAngularVelocity( const float dt ) noexcept
{
	// v = u + at
	_angularVelocity += _angularAcceleration * dt;
}

void RigidBody::ComputeOrientation( const float dt ) noexcept
{
	XMVECTOR velocity = XMLoadFloat3( &XMFLOAT3( _angularVelocity[0], _angularVelocity[1], _angularVelocity[2] ) );

	XMVECTOR orientationChange = velocity * dt;
	XMVECTOR oldOrientation = GetTransform()->GetOrientation();
	XMVECTOR newOrientation = XMQuaternionNormalize( XMQuaternionMultiply( orientationChange, oldOrientation ) );

	GetTransform()->SetOrientationQuaternion( newOrientation );
}

v3df RigidBody::GetForceAtRelativePosition( v3df position ) noexcept
{
	XMMATRIX inverseRotation = XMMatrixInverse( nullptr, GetTransform()->GetRotationMatrix() );
	XMVECTOR positionVec = XMLoadFloat3( &XMFLOAT3( position[0], position[1], position[2] ) );
	XMVECTOR relativePosition = XMVector3Transform( positionVec, inverseRotation );
	
	float magnitude = position.magnitude();
	XMVECTOR angularVelocityVec = XMLoadFloat3( &XMFLOAT3( _angularVelocity[0], _angularVelocity[1], _angularVelocity[2] ) );
	XMVECTOR localForce = XMVector3Normalize( XMVector3Cross( XMVector3Transform( angularVelocityVec, _inertiaTensor ), positionVec ) ) * magnitude;

	XMVECTOR velocity = XMLoadFloat3( &XMFLOAT3( GetVelocity()[0], GetVelocity()[1], GetVelocity()[2] ) );
	XMVECTOR relativeForce = XMVector3Transform( localForce, GetTransform()->GetRotationMatrix() ) + GetMass() * velocity;
	v3df relativeForceVec = { XMVectorGetX( relativeForce ), XMVectorGetY( relativeForce ), XMVectorGetZ( relativeForce ) };
	return relativeForceVec;
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