#include "stdafx.h"
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	_particles.reserve( MAX_PARTICLE_COUNT );
}

void ParticleSystem::Update( float deltaTime )
{
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

int ParticleSystem::FindNextAvailableParticleIndex()
{
	for ( int i = 0; i < MAX_PARTICLE_COUNT; i++ )
		if ( !_particles[i].GetActive() )
			return i;
	return -1;
}

void ParticleSystem::AddParticle( v3df position, v3df velocity, float maxAge )
{
	int nextIndex = FindNextAvailableParticleIndex();
	if ( nextIndex != -1 )
		_particles[nextIndex].Spawn( position, velocity, maxAge );
}

void ParticleSystem::DestroyParticle( int index )
{
	_particles.erase( _particles.begin() + index );
}