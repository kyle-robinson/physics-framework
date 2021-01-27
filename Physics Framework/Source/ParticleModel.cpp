#include "stdafx.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( std::shared_ptr<Transform> transform ) : _transform( transform )
{
	_mass = 50.0f;
	_floorHeight = 0.5f;
	_velocity = { 0.0f, 0.0f, 0.0f };
	_netForce = { 0.0f, 0.0f, 0.0f };
	_acceleration = { 0.0f, 0.0f, 0.0f };
}

/*void const ParticleModel::MoveForward()
{
	ResetForces();
	_force[2] = MOVEMENT_SPEED;
	_friction[2] = -( NORMAL_FORCE * FRICTION_COEF * _mass );
}

void const ParticleModel::MoveBackward()
{
	ResetForces();
	_force[2] = -MOVEMENT_SPEED;
	_friction[2] = std::abs( NORMAL_FORCE * FRICTION_COEF * _mass );
}

void const ParticleModel::MoveLeft()
{
	ResetForces();
	_force[0] = -MOVEMENT_SPEED;
	_friction[0] = std::abs( NORMAL_FORCE * FRICTION_COEF * _mass );
}

void const ParticleModel::MoveRight()
{
	ResetForces();
	_force[0] = MOVEMENT_SPEED;
	_friction[0] = -( NORMAL_FORCE * FRICTION_COEF * _mass );
}

void const ParticleModel::MoveUp()
{
	_enableThrust = true;
	
	ResetForces();
	_force[1] = MOVEMENT_SPEED * _thrust;
}*/

/*void ParticleModel::MoveConstVelocity( const float deltaTime )
{
	_transform->SetPosition(
		_transform->GetPosition()[0] + _velocity[0] * TIME_STEP,
		_transform->GetPosition()[1] + _velocity[1] * TIME_STEP,
		_transform->GetPosition()[2] + _velocity[2] * TIME_STEP
	);
}

void ParticleModel::MoveConstAcceleration( const float deltaTime )
{
	v3df position = _transform->GetPosition();

	position[0] += _velocity[0] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[0] * ( TIME_STEP * 0.00001f ) * ( TIME_STEP * 0.000001f );
	position[1] += _velocity[1] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[1] * ( TIME_STEP * 0.00001f ) * ( TIME_STEP * 0.000001f );
	position[2] += _velocity[2] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[2] * ( TIME_STEP * 0.00001f ) * ( TIME_STEP * 0.000001f );

	_velocity[0] += _acceleration[0] * ( deltaTime * 0.000001f );
	_velocity[1] += _acceleration[1] * ( deltaTime * 0.000001f );
	_velocity[2] += _acceleration[2] * ( deltaTime * 0.000001f );

	_transform->SetPosition( position );
}*/

/*void ParticleModel::ResetForces()
{
	_force = { 0.0f, 0.0f, 0.0f };
	_friction = { 0.0f, 0.0f, 0.0f };
}*/

void ParticleModel::Update( float deltaTime )
{
	ApplyGravity( deltaTime );
	ApplyThrust( deltaTime );

	ComputeAcceleration( deltaTime );
	ComputeVelocity( deltaTime );
	ComputePosition( deltaTime );

	CheckFloorCollision();

	_netForce = { 0.0f, 0.0f, 0.0f };
}

void ParticleModel::ApplyGravity( float deltaTime )
{
	_netForce[1] -= GRAVITY * _mass;
}

void ParticleModel::ApplyThrust( float deltaTime )
{

}

void ParticleModel::ComputeAcceleration( float deltaTime )
{
	_acceleration = _netForce / _mass;

	//_acceleration[0] = ( _acceleration[0] > MAX_ACCELERATION ) ? MAX_ACCELERATION : _acceleration[0];
	//_acceleration[1] = ( _acceleration[1] > MAX_ACCELERATION ) ? MAX_ACCELERATION : _acceleration[1];
	//_acceleration[2] = ( _acceleration[2] > MAX_ACCELERATION ) ? MAX_ACCELERATION : _acceleration[2];
}

void ParticleModel::ComputeVelocity( float deltaTime )
{
	_velocity += _acceleration * deltaTime;
}

void ParticleModel::ComputePosition( float deltaTime )
{
	_transform->SetPosition(
		_transform->GetPosition()[0] + _velocity[0] * deltaTime + 0.5f * _acceleration[0] * deltaTime * deltaTime,
		_transform->GetPosition()[1] + _velocity[1] * deltaTime + 0.5f * _acceleration[1] * deltaTime * deltaTime,
		_transform->GetPosition()[2] + _velocity[2] * deltaTime + 0.5f * _acceleration[2] * deltaTime * deltaTime
	);
}

void ParticleModel::CheckFloorCollision()
{
	v3df position = _transform->GetPosition();

	if ( position[1] < _floorHeight )
	{
		_velocity = { _velocity[0], -0.3f * _velocity[1], _velocity[2] };
		_transform->SetPosition( position[0], _floorHeight, position[2] );
	}

	if ( position[0] < -10.0f || position[0] > 10.0f )
		_velocity = { -_velocity[0], _velocity[1], _velocity[2] };

	if ( position[2] < -10.0f || position[2] > 10.0f )
		_velocity = { _velocity[0], _velocity[1], -_velocity[2] };
}