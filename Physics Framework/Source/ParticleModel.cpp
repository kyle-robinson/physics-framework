#include "stdafx.h"
#include "ParticleModel.h"

ParticleModel::ParticleModel( const std::string& name, bool usePhysics, bool useConstAccel )
	: _usePhysics( usePhysics ), _useConstAccel( useConstAccel ),
	_velocity( 0.0f, 0.0001f, 0.0f ), _acceleration( 0.0f, 0.0001f, 0.0f ), GameObject( name )
{}

ParticleModel::ParticleModel( const std::string& name, bool usePhysics, bool useConstAccel, v3df initialVelocity, v3df initialAcceleration )
	: _usePhysics( usePhysics ), _useConstAccel( useConstAccel ),
	_velocity( initialVelocity ), _acceleration( initialAcceleration ), GameObject( name )
{}

void ParticleModel::Update( float deltaTime )
{
	/*if ( _usePhysics )
	{
		if ( _useConstAccel )
			MoveConstVelocity( deltaTime );
		else
			MoveConstAcceleration( deltaTime );
	}*/

	GameObject::Update( deltaTime );
}

void ParticleModel::Draw( ID3D11DeviceContext* pImmediateContext )
{
	GameObject::Draw( pImmediateContext );
}

void ParticleModel::MoveConstVelocity( const float deltaTime )
{
	SetPosition(
		GetPosition()[0] + _velocity[0] * ( deltaTime * 0.000001f ),
		GetPosition()[1] + _velocity[1] * ( deltaTime * 0.000001f ),
		GetPosition()[2] + _velocity[2] * ( deltaTime * 0.000001f )
	);
}

void ParticleModel::MoveConstAcceleration( const float deltaTime )
{
	v3df position = GetPosition();

	position[0] += _velocity[0] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[0] * ( deltaTime * 0.00001f ) * ( deltaTime * 0.000001f );
	position[1] += _velocity[1] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[1] * ( deltaTime * 0.00001f ) * ( deltaTime * 0.000001f );
	position[2] += _velocity[2] * ( deltaTime * 0.000001f ) + 0.5f * _acceleration[2] * ( deltaTime * 0.00001f ) * ( deltaTime * 0.000001f );

	_velocity[0] += _acceleration[0] * ( deltaTime * 0.000001f );
	_velocity[1] += _acceleration[1] * ( deltaTime * 0.000001f );
	_velocity[2] += _acceleration[2] * ( deltaTime * 0.000001f );

	SetPosition( position );
}