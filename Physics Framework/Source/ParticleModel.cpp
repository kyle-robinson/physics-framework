#include "stdafx.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( std::shared_ptr<Transform> transform, bool useLaminar, v3df initialVelocity, v3df initialAccel )
	: _transform( transform ), _useLaminar( useLaminar ), _velocity( initialVelocity ), _acceleration( initialAccel )
{
	_mass = 50.0f;
	
	// f = m * g
	_weight = _mass * GRAVITY;

	_drag = { 0.0f, 0.0f, 0.0f };
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

void ParticleModel::Update()
{
	Weight();
	DragForce();
	Acceleration();
	Friction();
	Velocity();
	ComputePosition();
	CheckWorldCollisions();

	_netForce = { 0.0f, 0.0f, 0.0f };
}

void ParticleModel::Weight()
{
	_netForce[1] -= _weight * LIMITER;
	if ( _transform->GetPosition()[1] <= _transform->GetInitialPosition()[1] )
		_netForce[1] += _weight * LIMITER;
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
	_drag[0] = -DRAG_FACTOR * _velocity[0];
	_drag[1] = -DRAG_FACTOR * _velocity[1];
	_drag[2] = -DRAG_FACTOR * _velocity[2];
}

void ParticleModel::DragTurbulent()
{
	// magnitude of drag force
	v3df dragMag;
	static float density = 6.25f;
	dragMag[0] = 0.5f * density * -DRAG_FACTOR * 8.0f * _velocity[0] * _velocity[0];
	dragMag[1] = 0.5f * density * -DRAG_FACTOR * 8.0f * _velocity[1] * _velocity[1];
	dragMag[2] = 0.5f * density * -DRAG_FACTOR * 8.0f * _velocity[2] * _velocity[2];

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

void ParticleModel::Friction()
{
	// f = u * N
	v3df invVelocity = { -_velocity[0], -_velocity[1], -_velocity[2] };
	if ( _velocity.magnitude() < FRICTION * TIME_STEP )
		_friction = ( invVelocity / TIME_STEP );
	else
		_friction = invVelocity.normalize() * FRICTION;
}

void ParticleModel::Velocity()
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
	
	// floor/ceiling collisions
	if ( position[1] < _transform->GetInitialPosition()[1] )
	{
		_velocity = { _velocity[0], 0.0f, _velocity[2] };
		_transform->SetPosition( { position[0], _transform->GetInitialPosition()[1], position[2] } );
	}
	else if ( position[1] > 5.0f )
		_velocity[1] = -0.01f;

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