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
	_netForce = { x, y, z };
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
	_netForce.y -= _weight * _limiter;
	if ( _transform->GetPosition().y <= _transform->GetInitialPosition().y )
		_netForce.y += _weight * _limiter;
}

void ParticleModel::DragForce()
{
	if ( _useLaminar )
		DragLaminar();
	else
		DragTurbulent();

	//_netForce += _drag;
	_netForce -= _drag * _velocity;
}

void ParticleModel::DragLaminar()
{
	_drag = _velocity * -_dragFactor;
}

void ParticleModel::DragTurbulent()
{
	// magnitude of drag force
	v3df dragMag;
	static float density = 6.25f;
	dragMag.x = 0.5f * density * -_dragFactor * 8.0f * _velocity.x * _velocity.x;
	dragMag.y = 0.5f * density * -_dragFactor * 8.0f * _velocity.y * _velocity.y;
	dragMag.z = 0.5f * density * -_dragFactor * 8.0f * _velocity.z * _velocity.z;

	// adjust for negative movements
	if ( _velocity.x < 0.0f ) dragMag.x = -dragMag.x;
	if ( _velocity.y < 0.0f ) dragMag.y = -dragMag.y;
	if ( _velocity.z < 0.0f ) dragMag.z = -dragMag.z;

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
	v3df invVelocity = { -_velocity.x, -_velocity.y, -_velocity.z };
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
	if( _velocity.x > 0.0f )
		_velocity.x -= _frictionMultiplier;
	else if( _velocity.x < 0.0f )
		_velocity.x += _frictionMultiplier;

	// y-axis friction
	if( _velocity.y > 0.0f )
		_velocity.y -= _frictionMultiplier;
	else if( _velocity.y < 0.0f )
		_velocity.y += _frictionMultiplier;

	// z-axis friction
	if( _velocity.z > 0.0f )
		_velocity.z -= _frictionMultiplier;
	else if( _velocity.z < 0.0f )
		_velocity.z += _frictionMultiplier;
}

void ParticleModel::ComputePosition( const float dt )
{
	v3df position = _transform->GetPosition();
	position.x += _velocity.x * dt + 0.5f * _acceleration.x * dt * dt;
	position.y += _velocity.y * dt + 0.5f * _acceleration.y * dt * dt;
	position.z += _velocity.z * dt + 0.5f * _acceleration.z * dt * dt;
	_velocity += _acceleration * dt;
	_transform->SetPosition( position );
}

void ParticleModel::CheckWorldCollisions()
{
	v3df position = _transform->GetPosition();
	
	// floor/ceiling collisions
	if ( position.y < _transform->GetInitialPosition().y )
	{
		_velocity = { _velocity.x, 0.0f, _velocity.z };
		_transform->SetPosition( { position.x, _transform->GetInitialPosition().y, position.z } );
	}
	else if ( position.y > 7.5f )
		_velocity.y = -0.05f;

	// left/right collisions
	if ( position.x < -6.5f )
		_velocity.x = 0.01f;
	else if ( position.x > 6.5f )
		_velocity.x = -0.01f;

	// front/back collisions
	if ( position.z < 0.0f )
		_velocity.z = 0.01f;
	else if ( position.z > 15.0f )
		_velocity.z = -0.01f;
}

void ParticleModel::ResetForces()
{
	_velocity = { 0.0f, 0.0f, 0.0f };
	_acceleration = { 0.0f, 0.0f, 0.0f };
}