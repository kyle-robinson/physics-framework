#include "stdafx.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter( std::shared_ptr<ParticleSystem> particleSystem,
	v3df position, v3df direction, float directionRandomness, float power, float powerRandomModifier,
	float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier )
{
	_particleSystem = particleSystem;
	
	_position = position;
	_direction = direction;
	_directionRandomness = directionRandomness;
	
	_power = power;
	_powerRandomModifier = powerRandomModifier;
	
	_maxAge = maxAge;
	_maxAgeRandomModifier = maxAgeRandomModifier;
	
	_spawnDelay = spawnDelay;
	_spawnDelayRandomModifier = spawnDelayRandomModifier;

	_spawnTimer = 0.0f;
}

void ParticleEmitter::Update( float deltaTime )
{
	_spawnTimer -= deltaTime;
	if ( _spawnTimer < 0.0f )
	{
		_spawnTimer += _spawnDelay * ( 1.0f + _spawnDelayRandomModifier * static_cast<float>( rand() % 200 - 100 ) * 0.01f );
		SpawnParticle();
	}
}

void ParticleEmitter::SpawnParticle()
{
	v3df newPosition = _position;
	float newPower = _power * ( 1.0f + _powerRandomModifier * static_cast<float>( rand() % 200 - 100 ) * 0.01f );
	v3df randomVec = {
		static_cast<float>( rand() % 200 - 100 ) * 0.01f,
		static_cast<float>( rand() % 200 - 100 ) * 0.01f,
		static_cast<float>( rand() % 200 - 100 ) * 0.01f
	};

	XMVECTOR randomVecXM = { randomVec[0], randomVec[1], randomVec[2], 0.0f };
	XMVECTOR directionXM = { _direction[0], _direction[1], _direction[2], 0.0f };
	XMVECTOR newVelocityXM = newPower * XMVector3Normalize( XMVectorLerp( directionXM, randomVecXM, _directionRandomness ) );
	v3df newVelocity = { XMVectorGetX( newVelocityXM ), XMVectorGetY( newVelocityXM ), XMVectorGetZ( newVelocityXM ) };

	float newMaxAge = _maxAge * ( 1.0f + _maxAgeRandomModifier * static_cast<float>( rand() & 200 - 100 ) * 0.01f );
	_particleSystem->AddParticle( newPosition, newVelocity, newMaxAge );
}