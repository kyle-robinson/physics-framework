#include "stdafx.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( std::shared_ptr<Transform> transform, bool useConstAccel, v3df initialVelocity, v3df initialAccel )
	: _transform( transform ), _useConstAccel( useConstAccel ), _velocity( initialVelocity ), _acceleration( initialAccel )
{
	_mass = 50.0f;
	
	// f = m * g
	_weight = _mass * GRAVITY;

	_friction = { 0.0f, 0.0f, 0.0f };
	_netForce = { 0.0f, 0.0f, 0.0f };
	_emitterPosition = { 0.0f, 0.0f, 0.0f };
}

void ParticleModel::Move( float x, float y, float z )
{
	_netForce[0] += x;
	_netForce[1] += y;
	_netForce[2] += z;
}

void ParticleModel::Update( float deltaTime )
{
	ComputeWeight();
	ComputeAcceleration();
	ComputeFriction();
	ComputeVelocity();
	ComputePosition();
	CheckWorldCollisions();

	_netForce = { 0.0f, 0.0f, 0.0f };
}

void ParticleModel::ComputeWeight()
{
	_netForce[1] -= _weight * LIMITER;
	if ( _transform->GetPosition()[1] <= _transform->GetInitialPosition()[1] )
		_netForce[1] += _weight * LIMITER;
}

void ParticleModel::ComputeAcceleration()
{
	// f = m * a   ->   a = f / m
	_acceleration = _netForce / _mass;
}

void ParticleModel::ComputeFriction()
{
	// f = u * N
	v3df invVelocity = { -_velocity[0], -_velocity[1], -_velocity[2] };
	if ( _velocity.magnitude() < FRICTION * TIME_STEP )
		_friction = ( invVelocity / TIME_STEP );
	else
		_friction = invVelocity.normalize() * FRICTION;
}

void ParticleModel::ComputeVelocity()
{
	// v = u + at
	_velocity += _acceleration * TIME_STEP;

	// x-axis friction
	if( _velocity[0] > 0.0f )
		_velocity[0] -= FRICTION;
	else if( _velocity[0] < 0.0f )
		_velocity[0] += FRICTION;

	// y-axis friction
	if( _velocity[1] > 0.0f )
		_velocity[1] -= FRICTION;
	else if( _velocity[1] < 0.0f )
		_velocity[1] += FRICTION;

	// z-axis friction
	if( _velocity[2] > 0.0f )
		_velocity[2] -= FRICTION;
	else if( _velocity[2] < 0.0f )
		_velocity[2] += FRICTION;
}

void ParticleModel::ComputePosition()
{
	v3df position = _transform->GetPosition();
	position[0] += _velocity[0] * TIME_STEP + 0.5f * _acceleration[0] * TIME_STEP * TIME_STEP;
	position[1] += _velocity[1] * TIME_STEP + 0.5f * _acceleration[1] * TIME_STEP * TIME_STEP;
	position[2] += _velocity[2] * TIME_STEP + 0.5f * _acceleration[2] * TIME_STEP * TIME_STEP;
	_velocity += _acceleration * TIME_STEP;
	_transform->SetPosition( position );
}

void ParticleModel::CheckWorldCollisions()
{
	v3df position = _transform->GetPosition();
	
	// floor collision
	if ( position[1] < _transform->GetInitialPosition()[1] )
	{
		_velocity = { _velocity[0], 0.0f, _velocity[2] };
		_transform->SetPosition( { position[0], _transform->GetInitialPosition()[1], position[2] } );
	}

	// left/right collisions
	if ( position[0] < -6.5f )
		_velocity[0] = 0.01f;
	else if ( position[0] > 6.5f )
		_velocity[0] = -0.01f;

	// front/back collisions
	if ( position[2] < 0.0f )
		_velocity[2] = 0.01f;
	else if ( position[2] > 15.0f )
		_velocity[2] = -0.01f;
}

void ParticleModel::ResetForces()
{
	_velocity = { 0.0f, 0.0f, 0.0f };
	_acceleration = { 0.0f, 0.0f, 0.0f };
}