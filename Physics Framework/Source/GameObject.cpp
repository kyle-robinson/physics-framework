#include "stdafx.h"
#include "GameObject.h"

GameObject::GameObject( const std::string& type ) : _type( type )
{
	_transform = std::make_shared<Transform>();
	_appearance = std::make_shared<Appearance>();
	_particleModel = std::make_shared<ParticleModel>( _transform );
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

	if ( _transform->GetPosition()[1] < _transform->GetInitialPosition()[1] )
	{
		_transform->SetPosition( _transform->GetPosition()[0], 0.5f, _transform->GetPosition()[2] );
		_particleModel->SetAcceleration( _particleModel->GetAcceleration()[0], 0.5f, _particleModel->GetAcceleration()[2] );
		_particleModel->SetNetForce( _particleModel->GetNetForce()[0], 0.5f, _particleModel->GetNetForce()[2] );
		_particleModel->SetVelocity( _particleModel->GetVelocity()[0], 0.5f, _particleModel->GetVelocity()[2] );
	}
}

void GameObject::Draw( ID3D11DeviceContext* pImmediateContext )
{
	pImmediateContext->IASetVertexBuffers( 0, 1, &_appearance->_geometry.vertexBuffer,
		&_appearance->_geometry.vertexBufferStride, &_appearance->_geometry.vertexBufferOffset );
	pImmediateContext->IASetIndexBuffer( _appearance->_geometry.indexBuffer, DXGI_FORMAT_R16_UINT, 0 );
	pImmediateContext->DrawIndexed( _appearance->_geometry.numberOfIndices, 0, 0 );
}