#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject( const std::string& type ) : _type( type )
{
	_transform = std::make_shared<Transform>();
	_appearance = std::make_shared<Appearance>();
	_particleModel = std::make_shared<ParticleModel>( _transform, false, v3df( 0.0f, 0.0f, 0.0f ), v3df( 0.0f, 0.0f, 0.0f ) );
}

void GameObject::Update()
{
	_particleModel->Update();
	_transform->Update();
	if ( _transform->_parent != nullptr )
			XMStoreFloat4x4( &_transform->_world, _transform->GetWorldMatrix() * _transform->_parent->GetTransform()->GetWorldMatrix() );
}

void GameObject::Draw( ID3D11DeviceContext* pImmediateContext )
{
	_appearance->Update( pImmediateContext );
}