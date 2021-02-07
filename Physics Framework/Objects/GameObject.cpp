#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject( const std::string& id ) : _id( id )
{
	_transform = std::make_shared<Transform>();
	_appearance = std::make_shared<Appearance>();
	_particleModel = std::make_shared<ParticleModel>( _transform );
}

void GameObject::Update( const float dt )
{
	_particleModel->Update( dt );
	_transform->Update();
	if ( _transform->_parent != nullptr )
			XMStoreFloat4x4( &_transform->_worldMatrix, _transform->GetWorldMatrix() * _transform->_parent->GetTransform()->GetWorldMatrix() );
}

void GameObject::Draw( ID3D11DeviceContext* pImmediateContext )
{
	_appearance->Update( pImmediateContext );
}