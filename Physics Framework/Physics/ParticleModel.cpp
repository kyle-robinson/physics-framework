#include "stdafx.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( std::shared_ptr<Transform> transform ) : _transform( transform )
{
	_mass = 50.0f;
	_useLaminar = true;
	_isParticle = false;
	
	// f = m * g
	_weight = _mass * _gravity;

	_drag = 2.0f;
	_friction = { 0.0f, 0.0f, 0.0f };
	_netForce = { 0.0f, 0.0f, 0.0f };
	_velocity = { 0.0f, 0.0f, 0.0f };
	_acceleration = { 0.0f, 0.0f, 0.0f };
	_emitterPosition = { 0.0f, 0.0f, 0.0f };
}

bool ParticleModel::CollisionCheckAABB( v3df position )
{
	static float offset = 0.5f;
	if ( ( GetTransform()->GetPosition().x - offset <= position.x + offset &&
		   GetTransform()->GetPosition().x + offset >= position.x - offset ) &&
		( GetTransform()->GetPosition().y - offset <= position.y + offset &&
			GetTransform()->GetPosition().y + offset >= position.y - offset ) &&
		( GetTransform()->GetPosition().z - offset <= position.z + offset &&
			GetTransform()->GetPosition().z + offset >= position.z - offset )
	   )
	{
		return true;
	}
	else
		return false;
}

bool ParticleModel::CollisionCheckCircle( v3df position, float radius )
{
	if ( ( GetTransform()->GetPosition().x - position.x ) *
		 ( GetTransform()->GetPosition().x - position.x ) +
		 ( GetTransform()->GetPosition().y - position.y ) *
		 ( GetTransform()->GetPosition().y - position.y ) +
		 ( GetTransform()->GetPosition().z - position.z ) *
		 ( GetTransform()->GetPosition().z - position.z ) <= radius * radius )
		 return true;
	else
		 return false;
}

void ParticleModel::Move( float x, float y, float z )
{
	_netForce = { x, y, z };
}

void ParticleModel::Update( const float dt )
{
	Weight();
	ApplyThrust( dt );
	Acceleration();
	Friction( dt );
	Velocity( dt );
	DragForce( dt );
	ComputePosition( dt );
	CheckWorldCollisions();

	_netForce = { 0.0f, 0.0f, 0.0f };
}

void ParticleModel::Weight()
{
	_netForce.y -= _weight * _limiter;
}

void ParticleModel::ApplyThrust( const float dt )
{
	std::vector<int> deadForces;

	for ( int i = 0; i < _thrustForces.size(); ++i )
	{
		_netForce += _thrustForces[i].first;

		_thrustForces[i].second -= dt;

		if ( _thrustForces[i].second <= 0.0f )
			deadForces.push_back( i );
	}

	// Remove dead forces
	for ( int i = deadForces.size() - 1; i >= 0; --i )
		_thrustForces.erase( _thrustForces.begin() + deadForces[i] );
}

void ParticleModel::AddThrust( v3df force, float duration )
{
	_thrustForces.push_back( std::make_pair( force, duration ) );
}

void ParticleModel::DragForce( const float dt )
{
	if ( !_isParticle )
	{		
		// calculate drag factor
		if ( _useLaminar ) DragLaminar();
		else DragTurbulent();

		// add to velocity and adjust for negative values
		if ( _netForce.x < 0.0f ) _velocity.x *= 1.0f + _netForce.x;
		else _velocity.x *= 1.0f - _netForce.x;

		if ( _netForce.y < 0.0f ) _velocity.y *= 1.0f + _netForce.y;

		if ( _netForce.z < 0.0f ) _velocity.z *= 1.0f + _netForce.z;
		else _velocity.z *= 1.0f - _netForce.z;
	}
}

void ParticleModel::DragLaminar()
{
	_netForce -= _velocity * _drag;
}

void ParticleModel::DragTurbulent()
{
	// magnitude of drag force
	float velocityMagnitude = _velocity.magnitude();
	v3df unitVelocity = _velocity.normalize();

	// calculate new drag factor
	float dragMagnitude = _drag * velocityMagnitude * velocityMagnitude;
	v3df newVelocity = unitVelocity * dragMagnitude;

	// adjust for negative values
	if ( newVelocity.x < 0.0f ) _netForce.x += newVelocity.x;
	else _netForce.x -= newVelocity.x;

	if ( newVelocity.y < 0.0f ) _netForce.y += newVelocity.y;
	else _netForce.y -= newVelocity.y;

	if ( newVelocity.z < 0.0f ) _netForce.z += newVelocity.z;
	else _netForce.z -= newVelocity.z;
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
	if ( _isParticle ) _velocity *= 0.75f;

	// v = u + at
	_velocity += _acceleration * dt;

	// x-axis friction
	if( _velocity.x > 0.0f ) _velocity.x -= _frictionMultiplier;
	else if( _velocity.x < 0.0f ) _velocity.x += _frictionMultiplier;

	// y-axis friction
	if( _velocity.y > 0.0f ) _velocity.y -= _frictionMultiplier;
	else if( _velocity.y < 0.0f ) _velocity.y += _frictionMultiplier;

	// z-axis friction
	if( _velocity.z > 0.0f ) _velocity.z -= _frictionMultiplier;
	else if( _velocity.z < 0.0f ) _velocity.z += _frictionMultiplier;
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
	{
		_velocity.y = -0.05f;
	}

	// left/right collisions
	if ( position.x < -6.5f ) _velocity.x = 0.01f;
	else if ( position.x > 6.5f ) _velocity.x = -0.01f;

	// front/back collisions
	if ( position.z < 0.0f ) _velocity.z = 0.01f;
	else if ( position.z > 15.0f ) _velocity.z = -0.01f;
}

void ParticleModel::ResetForces()
{
	_netForce = { 0.0f, 0.0f, 0.0f };
	_velocity = { 0.0f, 0.0f, 0.0f };
	_acceleration = { 0.0f, 0.0f, 0.0f };
}