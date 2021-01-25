#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject( const std::string& type, Appearance* appearance, Transform* transform, ParticleModel* particleModel )
	: _type( type ), _appearance( appearance ), _transform( transform ), _particleModel( particleModel )
{}

void GameObject::Update( float deltaTime )
{
	_particleModel->Update( deltaTime );
	_transform->Update( deltaTime );
}

void GameObject::Draw( ID3D11DeviceContext* pImmediateContext )
{
	_appearance->Draw( pImmediateContext );
}