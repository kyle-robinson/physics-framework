#include "stdafx.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( std::shared_ptr<Transform> transform ) : _transform( transform )
{}

void ParticleModel::MoveConstVelocity( const float deltaTime )
{
	static float timeStep = 1.5f;
	_transform->SetPosition(
		_transform->GetPosition()[0] + _velocity[0] * timeStep,
		_transform->GetPosition()[1] + _velocity[1] * timeStep,
		_transform->GetPosition()[2] + _velocity[2] * timeStep
	);
}

void ParticleModel::MoveConstAcceleration( const float deltaTime )
{
	static float timeStep = 0.1f;
	v3df position = _transform->GetPosition();

	position[0] += _velocity[0] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[0] * ( timeStep * 0.00001f ) * ( timeStep * 0.000001f );
	position[1] += _velocity[1] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[1] * ( timeStep * 0.00001f ) * ( timeStep * 0.000001f );
	position[2] += _velocity[2] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[2] * ( timeStep * 0.00001f ) * ( timeStep * 0.000001f );

	_velocity[0] += _acceleration[0] * ( deltaTime * 0.000001f );
	_velocity[1] += _acceleration[1] * ( deltaTime * 0.000001f );
	_velocity[2] += _acceleration[2] * ( deltaTime * 0.000001f );

	_transform->SetPosition( position );
}