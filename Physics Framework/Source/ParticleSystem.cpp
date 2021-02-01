#include "stdafx.h"
#include "ParticleSystem.h"

/*   PARTICLE SYSTEM   */
ParticleSystem::ParticleSystem( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureStone,
	Geometry geometry, Material material )
{
	for ( int i = 0; i < MAX_PARTICLE_COUNT; ++i )
	{
		_particles[i].GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
		_particles[i].GetTransform()->SetInitialPosition( 0.0f, 0.5f, -2.5f );
		_particles[i].GetAppearance()->SetTextureRV( textureStone.Get() );
		_particles[i].GetAppearance()->SetGeometryData( geometry );
		_particles[i].GetAppearance()->SetMaterial( material );
	}
}

void ParticleSystem::Update( float deltaTime )
{
	for ( int i = 0; i < MAX_PARTICLE_COUNT; ++i )
		_particles[i].Update( deltaTime );
}

void ParticleSystem::Draw( ID3D11DeviceContext* pImmediateContext )
{
	v3df position;
	for ( int i = 0; i < MAX_PARTICLE_COUNT; ++i )
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
			_particles[i].GetAppearance()->Update( pImmediateContext );
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
	for ( int i = 0; i < MAX_PARTICLE_COUNT; ++i )
		if ( !_particles[i].GetActive() )
			return i;
	return -1;
}