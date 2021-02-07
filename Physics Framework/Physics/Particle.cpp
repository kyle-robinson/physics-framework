#include "stdafx.h"
#include "Particle.h"

Particle::Particle( const std::string& id ) : id( id )
{
	size = 0.01f;
	maxEnergy = 100;
	energy = maxEnergy;
	startTimer = rand() % ( maxEnergy * 10 );
	
	_transform = std::make_shared<Transform>();
	_transform->SetScale( 0.5f, 0.5f, 0.5f );
	_transform->SetInitialPosition( 0.0f, 0.5f, 0.5f );
	_particleModel = std::make_shared<ParticleModel>( _transform );
	_appearance = std::make_shared<Appearance>();
}

void Particle::Update( const float dt )
{

	if ( startTimer < 0 )
	{
		_particleModel->Update( dt );
		_transform->Update();
		_transform->SetScale( v3df( energy * size, energy * size, energy * size ) );
	}

	if ( energy <= 0 && startTimer < 0 )
		RespawnParticle();
	
	energy -= 0.1f;
	startTimer -= 1.0f;
}

void Particle::Draw( ID3D11DeviceContext* pImmediateContext )
{	
	if ( startTimer < 0 )
		_appearance->Update( pImmediateContext );
}

void Particle::RespawnParticle()
{
	energy = maxEnergy;
	_transform->SetPosition( _transform->GetInitialPosition() );
}