#include "stdafx.h"
#include "Transform.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( Transform* transform, bool useConstAccel, v3df initialVelocity, v3df initialAcceleration )
	: _transform( transform ), _velocity( initialVelocity ), _acceleration( initialAcceleration )
{}

void ParticleModel::Update( float deltaTime )
{
	if ( _useConstVelocity )
		MoveConstVelocity( deltaTime );
	else
		MoveConstAcceleration( deltaTime );
}

void ParticleModel::MoveConstVelocity( const float deltaTime )
{
	_transform->SetPosition(
		_transform->GetPosition()[0] + _velocity[0] * deltaTime,
		_transform->GetPosition()[1] + _velocity[1] * deltaTime,
		_transform->GetPosition()[2] + _velocity[2] * deltaTime
	);
}

void ParticleModel::MoveConstAcceleration( const float deltaTime )
{
	v3df position = _transform->GetPosition();

	position[0] += _velocity[0] * deltaTime + 0.5f * _acceleration[0] * deltaTime * deltaTime;
	position[1] += _velocity[1] * deltaTime + 0.5f * _acceleration[1] * deltaTime * deltaTime;
	position[2] += _velocity[2] * deltaTime + 0.5f * _acceleration[2] * deltaTime * deltaTime;

	_velocity[0] += _acceleration[0] * deltaTime;
	_velocity[1] += _acceleration[1] * deltaTime;
	_velocity[2] += _acceleration[2] * deltaTime;

	_transform->SetPosition( position );
}