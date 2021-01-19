#include "PCH.h"
#include "GameObject.h"

GameObject::GameObject( std::string type, Geometry geometry, Material material ) : _geometry( geometry ), _type( type ), _material( material )
{
	_parent = nullptr;
	_position = { };
	_rotation = { };
	_scale = { 1.0f, 1.0f, 1.0f };

	_textureRV = nullptr;
}

void GameObject::moveForward()
{
	v3df position = this->GetPosition();
	position[2] -= 0.02f;
	this->SetPosition( position );
}

void GameObject::moveBackward()
{
	v3df position = this->GetPosition();
	position[2] += 0.02f;
	this->SetPosition( position );
}

void GameObject::Update( float t )
{
	// Calculate world matrix
	XMMATRIX scale = XMMatrixScaling( _scale[0], _scale[1], _scale[2] );
	XMMATRIX rotation = XMMatrixRotationX( _rotation[0] ) * XMMatrixRotationY( _rotation[1] ) * XMMatrixRotationZ( _rotation[2] );
	XMMATRIX translation = XMMatrixTranslation( _position[0], _position[1], _position[2] );

	XMStoreFloat4x4( &_world, scale * rotation * translation );

	if ( _parent != nullptr )
		XMStoreFloat4x4( &_world, this->GetWorldMatrix() * _parent->GetWorldMatrix() );
}

void GameObject::Draw( ID3D11DeviceContext * pImmediateContext )
{
	// NOTE: We are assuming that the constant buffers and all other draw setup has already taken place

	// Set vertex and index buffers
	pImmediateContext->IASetVertexBuffers( 0, 1, &_geometry.vertexBuffer, &_geometry.vertexBufferStride, &_geometry.vertexBufferOffset );
	pImmediateContext->IASetIndexBuffer( _geometry.indexBuffer, DXGI_FORMAT_R16_UINT, 0 );

	pImmediateContext->DrawIndexed( _geometry.numberOfIndices, 0, 0 );
}