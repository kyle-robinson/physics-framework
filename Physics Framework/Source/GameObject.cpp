#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject( const std::string& type ) : _type( type )
{
	_transform = std::make_shared<Transform>();
	_transform->_parent = nullptr;
	_transform->SetPosition( 0.0f, 0.0f, 0.0f );
	_transform->SetRotation( 0.0f, 0.0f, 0.0f );
	_transform->SetScale( 1.0f, 1.0f, 1.0f );

	_appearance = std::make_shared<Appearance>();
	_appearance->_textureRV = nullptr;
	_appearance->_geometry = {};
	_appearance->_material = {};

	_particleModel = std::make_shared<ParticleModel>( _transform );
	_particleModel->_useConstAccel = true;
	_particleModel->_velocity = { 0.0f, 0.001f, 0.0f };
	_particleModel->_acceleration = { 0.0f, 0.001f, 0.0f };
}

void GameObject::Update( float deltaTime )
{
	// Calculate world matrix
	XMMATRIX scale = XMMatrixScaling( _transform->GetScale()[0], _transform->GetScale()[1], _transform->GetScale()[2] );
	XMMATRIX rotation = XMMatrixRotationX( _transform->GetRotation()[0] ) * XMMatrixRotationY( _transform->GetRotation()[1] ) * XMMatrixRotationZ( _transform->GetRotation()[2] );
	XMMATRIX translation = XMMatrixTranslation( _transform->GetPosition()[0], _transform->GetPosition()[1], _transform->GetPosition()[2] );

	XMStoreFloat4x4( &_transform->_world, scale * rotation * translation );

	if ( _transform->_parent != nullptr )
		XMStoreFloat4x4( &_transform->_world, _transform->GetWorldMatrix() * _transform->_parent->_transform->GetWorldMatrix() );
}

void GameObject::Draw( ID3D11DeviceContext* pImmediateContext )
{
	pImmediateContext->IASetVertexBuffers( 0, 1, &_appearance->_geometry.vertexBuffer, &_appearance->_geometry.vertexBufferStride, &_appearance->_geometry.vertexBufferOffset );
	pImmediateContext->IASetIndexBuffer( _appearance->_geometry.indexBuffer, DXGI_FORMAT_R16_UINT, 0 );
	pImmediateContext->DrawIndexed( _appearance->_geometry.numberOfIndices, 0, 0 );
}