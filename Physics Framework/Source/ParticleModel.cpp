#include "stdafx.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( std::shared_ptr<Transform> transform, bool useConstAccel, v3df initialVelocity, v3df initialAccel )
	: _transform( transform ), _useConstAccel( useConstAccel ), _velocity( initialVelocity ), _acceleration( initialAccel )
{
	_mass = 50.0f;
	_force = { 0.0f, 0.0f, 0.0f };
}

void ParticleModel::Move( float x, float y, float z )
{
	_force[0] = x * LIMITER;
	_force[1] = y * LIMITER;
	_force[2] = z * LIMITER;
}

void ParticleModel::MoveConstVelocity( float deltaTime )
{
	v3df position = _transform->GetPosition();
	position += _velocity * 0.01f;
	_transform->SetPosition( position );
}

void ParticleModel::MoveConstAcceleration( float deltaTime )
{
	v3df position = _transform->GetPosition();

	position[0] += _velocity[0] * ( deltaTime * LIMITER ) + 0.5f * _acceleration[0] * ( deltaTime * LIMITER ) * ( deltaTime * LIMITER );
	position[1] += _velocity[1] * ( deltaTime * LIMITER ) + 0.5f * _acceleration[1] * ( deltaTime * LIMITER ) * ( deltaTime * LIMITER );
	position[2] += _velocity[2] * ( deltaTime * LIMITER ) + 0.5f * _acceleration[2] * ( deltaTime * LIMITER ) * ( deltaTime * LIMITER );

	_velocity += _acceleration * deltaTime * LIMITER;
	_transform->SetPosition( position );
}

void ParticleModel::Update( float deltaTime )
{
	if ( _useConstAccel )
		MoveConstAcceleration( deltaTime );
	else
		MoveConstVelocity( deltaTime );

	//ApplyGravity();
	//ComputeVelocity( deltaTime );
	CheckFloorCollision();

	//_force = { 0.0f, 0.0f, 0.0f };
}

void ParticleModel::ApplyGravity()
{
	_force[1] += _mass * GRAVITY;
}

void ParticleModel::ComputeVelocity( float deltaTime )
{
	_velocity += _force / _mass * deltaTime;
	v3df position = _transform->GetPosition();
	position += _velocity * deltaTime;
	_transform->SetPosition( position );
}

void ParticleModel::CheckFloorCollision()
{
	v3df position = _transform->GetPosition();
	if ( position[1] < _transform->GetInitialPosition()[1] )
	{
		_velocity = { _velocity[0], 0.0f, _velocity[2] };
		_transform->SetPosition( { position[0], _transform->GetInitialPosition()[1], position[2] } );
	}
}

void ParticleModel::ResetForces()
{
	_velocity = { 0.0f, 0.0f, 0.0f };
	_acceleration = { 0.0f, 0.0f, 0.0f };
}