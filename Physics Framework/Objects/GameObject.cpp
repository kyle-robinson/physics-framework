#include "../PCH.h"
#include "GameObject.h"

GameObject::GameObject( std::string type, Geometry geometry, Material material ) : _type( type )
{
	_parent = nullptr;
	_textureRV = nullptr;
	_geometry = geometry;
	_material = material;

	SetPosition( 0.0f, 0.0f, 0.0f );
	SetRotation( 0.0f, 0.0f, 0.0f );
	SetScale( 1.0f, 1.0f, 1.0f );
}

void GameObject::Update( float t )
{
	// Calculate world matrix
	XMMATRIX scale = XMMatrixScaling( GetScale()[0], GetScale()[1], GetScale()[2] );
	XMMATRIX rotation = XMMatrixRotationX( GetRotation()[0] ) * XMMatrixRotationY( GetRotation()[1] ) * XMMatrixRotationZ( GetRotation()[2] );
	XMMATRIX translation = XMMatrixTranslation( GetPosition()[0], GetPosition()[1], GetPosition()[2] );

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