#include "stdafx.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( const std::string& name, bool useConstAccel, v3df initialVelocity, v3df initialAcceleration )
	: _useConstAccel( useConstAccel ), _velocity( initialVelocity ), _acceleration( initialAcceleration ), GameObject( name )
{}

void ParticleModel::Update( float deltaTime )
{
	if ( _useConstAccel )
		MoveConstVelocity( deltaTime );
	else
		MoveConstAcceleration( deltaTime );

	this->Update( deltaTime );
}

void ParticleModel::Draw( ID3D11DeviceContext* pImmediateContext )
{
	this->Draw( pImmediateContext );
}

void ParticleModel::MoveConstVelocity( const float deltaTime )
{
	SetPosition(
		GetPosition()[0] + _velocity[0] * deltaTime,
		GetPosition()[1] + _velocity[1] * deltaTime,
		GetPosition()[2] + _velocity[2] * deltaTime
	);
}

void ParticleModel::MoveConstAcceleration( const float deltaTime )
{
	v3df position = GetPosition();

	position[0] += _velocity[0] * deltaTime + 0.5f * _acceleration[0] * deltaTime * deltaTime;
	position[1] += _velocity[1] * deltaTime + 0.5f * _acceleration[1] * deltaTime * deltaTime;
	position[2] += _velocity[2] * deltaTime + 0.5f * _acceleration[2] * deltaTime * deltaTime;

	_velocity[0] += _acceleration[0] * deltaTime;
	_velocity[1] += _acceleration[1] * deltaTime;
	_velocity[2] += _acceleration[2] * deltaTime;

	SetPosition( position );
}