#include "stdafx.h"
#include "ParticleSystem.h"

/*   PARTICLE EMITTER */
ParticleEmitter::ParticleEmitter( ParticleSystem* particleSystem, v3df position, v3df direction, float directionRandomness,
	float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier,
	float spawnDelay, float spawnDelayRandomModifier )
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

/*   PARTICLE SYSTEM   */
ParticleSystem::ParticleSystem()
{
	_particles.reserve( MAX_PARTICLE_COUNT );
}

void ParticleSystem::Update( float deltaTime )
{
	for ( int i = 0; i < MAX_PARTICLE_COUNT; i++ )
		_particleEmitters[i]->Update( deltaTime );

	for ( int i = 0; i < MAX_PARTICLE_COUNT; i++ )
		_particles[i].Update( deltaTime );
}

void ParticleSystem::Draw()
{
	v3df position;
	for ( int i = 0; i < MAX_PARTICLE_COUNT; i++ )
	{
		if ( _particles[i].GetActive() )
		{
			float particleAge = _particles[i].GetAge();
			float particleMaxAge = _particles[i].GetMaxAge();
			float particleScale = 0.25f;

			if ( particleAge < particleMaxAge * 0.15f )
				particleScale *= particleAge / ( particleMaxAge * 0.15f );
			else if ( particleAge > particleMaxAge * 0.85f )
				particleScale *= 1.0f - ( ( particleAge - particleMaxAge * -0.85f ) / ( particleMaxAge * 0.15f ) );

			_particles[i].GetTransform()->SetPosition( position );
			_particles[i].GetTransform()->SetScale( { particleScale, particleScale, particleScale } );
		}
	}
}

void ParticleSystem::AddParticle( v3df position, v3df velocity, float maxAge )
{
	int nextIndex = FindNextAvailableParticleIndex();
	if ( nextIndex != -1 )
		_particles[nextIndex].Spawn( position, velocity, maxAge );
}

int ParticleSystem::FindNextAvailableParticleIndex()
{
	for ( int i = 0; i < MAX_PARTICLE_COUNT; i++ )
		if ( !_particles[i].GetActive() )
			return i;
	return -1;
}

void ParticleSystem::DestroyParticle( int index )
{
	_particles.erase( _particles.begin() + index );
}

void ParticleSystem::AddEmitter( v3df position, v3df direction, float directionRandomness, float power, float powerRandomModifier,
		float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier )
{
	_particleEmitters.push_back( new ParticleEmitter( this, position, direction, directionRandomness, power, powerRandomModifier,
		maxAge, maxAgeRandomModifier, spawnDelay, spawnDelayRandomModifier ) );
}