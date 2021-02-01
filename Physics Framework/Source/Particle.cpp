#include "stdafx.h"
#include "Particle.h"

Particle::Particle() { }

void Particle::Update( float deltaTime )
{
	if ( _active )
	{
		_particleModel->Update();
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