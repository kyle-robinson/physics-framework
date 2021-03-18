#include "stdafx.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( std::shared_ptr<Transform> transform ) : _transform( transform )
{
	_mass = 50.0f;
	_useLaminar = true;
	_isParticle = false;
	_ignoreWorldCollisions = false;

	_drag = 2.0f;
	_friction = { 0.0f, 0.0f, 0.0f };
	_netForce = { 0.0f, 0.0f, 0.0f };
	_velocity = { 0.0f, 0.0f, 0.0f };
	_acceleration = { 0.0f, 0.0f, 0.0f };
}

bool ParticleModel::CollisionCheckAABB( v3df position )
{
	static float offset = 0.5f;
	if ( ( _transform->GetPosition().x - offset <= position.x + offset && // x collisions
		   _transform->GetPosition().x + offset >= position.x - offset ) &&
		 ( _transform->GetPosition().y - offset <= position.y + offset && // y collisions
		   _transform->GetPosition().y + offset >= position.y - offset ) &&
		 ( _transform->GetPosition().z - offset <= position.z + offset && // z collisions
		   _transform->GetPosition().z + offset >= position.z - offset )
	   )
	{
		return true;
	}
	else
		return false;
}

bool ParticleModel::CollisionCheckCircle( v3df position, float radius )
{
	if ( ( _transform->GetPosition().x - position.x ) * // x collisions
		 ( _transform->GetPosition().x - position.x ) +
		 ( _transform->GetPosition().y - position.y ) * // y collisions
		 ( _transform->GetPosition().y - position.y ) +
		 ( _transform->GetPosition().z - position.z ) * // z collisions
		 ( _transform->GetPosition().z - position.z ) <= radius * radius )
		 return true;
	else
		 return false;
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
	if ( !_ignoreWorldCollisions )
		CheckWorldCollisions( dt );

	_netForce = { 0.0f, 0.0f, 0.0f };
}

void ParticleModel::Weight()
{
	// f = m * g
	_weight = _mass * _gravity;
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
		if ( _netForce.x < 0.0f ) // x values
			_velocity.x *= 1.0f + _netForce.x;
		else
			_velocity.x *= 1.0f - _netForce.x;

		if ( _netForce.y < 0.0f ) // y values
			_velocity.y *= 1.0f + _netForce.y;

		if ( _netForce.z < 0.0f ) // z values
			_velocity.z *= 1.0f + _netForce.z;
		else
			_velocity.z *= 1.0f - _netForce.z;
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
	if ( newVelocity.x < 0.0f ) // x values
		_netForce.x += newVelocity.x;
	else
		_netForce.x -= newVelocity.x;

	if ( newVelocity.y < 0.0f ) // y values
		_netForce.y += newVelocity.y;
	else
		_netForce.y -= newVelocity.y;

	if ( newVelocity.z < 0.0f ) // z values
		_netForce.z += newVelocity.z;
	else
		_netForce.z -= newVelocity.z;
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

	// adjust for negative values
	if( _velocity.x > 0.0f ) // x values
		_velocity.x -= _frictionMultiplier;
	else if( _velocity.x < 0.0f )
		_velocity.x += _frictionMultiplier;

	if( _velocity.y > 0.0f ) // y values
		_velocity.y -= _frictionMultiplier;
	else if( _velocity.y < 0.0f )
		_velocity.y += _frictionMultiplier;

	if( _velocity.z > 0.0f ) // z values
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

void ParticleModel::CheckWorldCollisions( const float dt )
{
	v3df position = _transform->GetPosition();

	// y collisions
	if ( position.y < _transform->GetInitialPosition().y )
	{
		_velocity.y = 0.0f;
		position.y = _transform->GetInitialPosition().y;
	}
	else if ( position.y > 9.5f )
	{
		_velocity.y = 0.0f;
		position.y = 9.5f;
	}

	// x collisions
	if ( position.x < -9.5f )
		position.x = -9.5f;
	else if ( position.x > 9.5f )
		position.x = 9.5f;

	// z collisions
	if ( position.z < -4.5f )
		position.z = -4.5f;
	else if ( position.z > 19.5f )
		position.z = 19.5f;

	_transform->SetPosition( position );
}

void ParticleModel::ResetForces()
{
	_netForce = { 0.0f, 0.0f, 0.0f };
}