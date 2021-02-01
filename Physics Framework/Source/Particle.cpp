#include "stdafx.h"
#include "Particle.h"

Particle::Particle()
{
	_transform = std::make_shared<Transform>();
	_appearance = std::make_shared<Appearance>();
	_particleModel = std::make_shared<ParticleModel>( _transform );
}

void Particle::Update( float deltaTime )
{
	if ( _active )
	{
		_particleModel->Update();
		_transform->Update();
		_curAge += deltaTime;
		if ( _curAge > _maxAge )
			_active = false;
	}
}

void Particle::Spawn( v3df position, v3df velocity, float maxAge )
{
	_transform->SetPosition( position );
	_particleModel->SetVelocity( velocity );
	_maxAge = maxAge;
	_curAge = 0.0f;
	_active = true;
}