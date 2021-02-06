#include "stdafx.h"
#include "Particle.h"
//#include "ParticleSystem.h"

Particle::Particle( const std::string& id ) : id( id )
{
	energy = 100.0f;
	respawnTimer = rand() % 1000;
	
	_transform = std::make_shared<Transform>();
	_transform->SetScale( 0.5f, 0.5f, 0.5f );
	_transform->SetInitialPosition( 0.0f, 0.5f, 0.5f );
	_particleModel = std::make_shared<ParticleModel>( _transform );
	_appearance = std::make_shared<Appearance>();
}

void Particle::Update()
{
	_particleModel->Update();
	_transform->Update();

	_transform->SetScale( v3df( energy * 0.01f, energy * 0.01f, energy * 0.01f ) );

	if ( energy <= 0.0f && respawnTimer <= 0 )
		Respawn();

	energy -= 0.1f;
	respawnTimer -= 1;

	/*if ( _active )
	{
		_particleModel->Update();
		_transform->Update();
		_curAge += deltaTime;
		if ( _curAge > _maxAge )
			_active = false;
	}*/
}

void Particle::Draw( ID3D11DeviceContext* pImmediateContext, ConstantBuffer<CB_VS_matrix>& cb_vs_matrix )
{	
	cb_vs_matrix.data.World = XMMatrixTranspose( GetTransform()->GetWorldMatrix() );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	_appearance->Update( pImmediateContext );
}

void Particle::Respawn()
{
	energy = 100.0f;
	_transform->SetPosition( _transform->GetInitialPosition() );
}

/*void Particle::Spawn( v3df position, v3df velocity, float maxAge )
{
	_transform->SetPosition( position );
	_particleModel->SetVelocity( velocity );
	_maxAge = maxAge;
	_curAge = 0.0f;
	_active = true;
}*/