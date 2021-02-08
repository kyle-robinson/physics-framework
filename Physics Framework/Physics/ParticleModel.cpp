#include "stdafx.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( std::shared_ptr<Transform> transform ) : _transform( transform )
{
	_mass = 50.0f;
	_useLaminar = false;
	
	// f = m * g
	_weight = _mass * _gravity;

	_drag = { 0.0f, 0.0f, 0.0f };
	_friction = { 0.0f, 0.0f, 0.0f };
	_netForce = { 0.0f, 0.0f, 0.0f };
	_velocity = { 0.0f, 0.0f, 0.0f };
	_acceleration = { 0.0f, 0.0f, 0.0f };
	_emitterPosition = { 0.0f, 0.0f, 0.0f };
}

void ParticleModel::Move( float x, float y, float z )
{
	_netForce[0] += x;
	_netForce[1] += y;
	_netForce[2] += z;
}

void ParticleModel::Update( const float dt )
{
	Weight();
	DragForce();
	Acceleration();
	Friction( dt );
	Velocity( dt );
	ComputePosition( dt );
	CheckWorldCollisions();

	_netForce = { 0.0f, 0.0f, 0.0f };
}

void ParticleModel::Weight()
{
	_netForce[1] -= _weight * _limiter;
	if ( _transform->GetPosition()[1] <= _transform->GetInitialPosition()[1] )
		_netForce[1] += _weight * _limiter;
}

void ParticleModel::DragForce()
{
	if ( _useLaminar )
		DragLaminar();
	else
		DragTurbulent();

	_netForce += _drag;
}

void ParticleModel::DragLaminar()
{
	_drag[0] = -_dragFactor * _velocity[0];
	_drag[1] = -_dragFactor * _velocity[1];
	_drag[2] = -_dragFactor * _velocity[2];
}

void ParticleModel::DragTurbulent()
{
	// magnitude of drag force
	v3df dragMag;
	static float density = 6.25f;
	dragMag[0] = 0.5f * density * -_dragFactor * 8.0f * _velocity[0] * _velocity[0];
	dragMag[1] = 0.5f * density * -_dragFactor * 8.0f * _velocity[1] * _velocity[1];
	dragMag[2] = 0.5f * density * -_dragFactor * 8.0f * _velocity[2] * _velocity[2];

	// adjust for negative movements
	if ( _velocity[0] < 0.0f )
		dragMag[0] = -dragMag[0];
	if ( _velocity[1] < 0.0f )
		dragMag[1] = -dragMag[1];
	if ( _velocity[2] < 0.0f )
		dragMag[2] = -dragMag[2];

	_drag = dragMag;
}

void ParticleModel::Acceleration()
{
	// f = m * a   ->   a = f / m
	_acceleration = _netForce / _mass;
}

void ParticleModel::Friction( const float dt )
{
	// f = u * N
	v3df invVelocity = { -_velocity[0], -_velocity[1], -_velocity[2] };
	if ( _velocity.magnitude() < _frictionMultiplier * dt )
		_friction = ( invVelocity / dt );
	else
		_friction = invVelocity.normalize() * _frictionMultiplier;
}

void ParticleModel::Velocity( const float dt )
{
	// v = u + at
	_velocity += _acceleration * dt;

	// x-axis friction
	if( _velocity[0] > 0.0f )
		_velocity[0] -= _frictionMultiplier;
	else if( _velocity[0] < 0.0f )
		_velocity[0] += _frictionMultiplier;

	// y-axis friction
	if( _velocity[1] > 0.0f )
		_velocity[1] -= _frictionMultiplier;
	else if( _velocity[1] < 0.0f )
		_velocity[1] += _frictionMultiplier;

	// z-axis friction
	if( _velocity[2] > 0.0f )
		_velocity[2] -= _frictionMultiplier;
	else if( _velocity[2] < 0.0f )
		_velocity[2] += _frictionMultiplier;
}

void ParticleModel::ComputePosition( const float dt )
{
	v3df position = _transform->GetPosition();
	position[0] += _velocity[0] * dt + 0.5f * _acceleration[0] * dt * dt;
	position[1] += _velocity[1] * dt + 0.5f * _acceleration[1] * dt * dt;
	position[2] += _velocity[2] * dt + 0.5f * _acceleration[2] * dt * dt;
	_velocity += _acceleration * dt;
	_transform->SetPosition( position );
}

void ParticleModel::CheckWorldCollisions()
{
	v3df position = _transform->GetPosition();
	
	// floor/ceiling collisions
	if ( position[1] < _transform->GetInitialPosition()[1] )
	{
		_velocity = { _velocity[0], 0.0f, _velocity[2] };
		_transform->SetPosition( { position[0], _transform->GetInitialPosition()[1], position[2] } );
	}
	else if ( position[1] > 7.5f )
		_velocity[1] = -0.05f;

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