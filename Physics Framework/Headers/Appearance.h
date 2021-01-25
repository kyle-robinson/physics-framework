#pragma once
#ifndef APPEARANCE_H
#define APPEARANCE_H

#include <d3d11.h>
#include <DirectXMath.h>

struct Material
{
	v4df diffuse;
	v4df ambient;
	v4df specular;
	float specularPower;
};

struct Geometry
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numberOfIndices;

	UINT vertexBufferStride;
	UINT vertexBufferOffset;
};

class Appearance
{
public:
	Appearance( Geometry geometry, Material material ) : _geometry( geometry ), _material( material ), _textureRV( nullptr ) {}
	void Draw( ID3D11DeviceContext * pImmediateContext )
	{
		pImmediateContext->IASetVertexBuffers( 0, 1, &_geometry.vertexBuffer, &_geometry.vertexBufferStride, &_geometry.vertexBufferOffset );
		pImmediateContext->IASetIndexBuffer( _geometry.indexBuffer, DXGI_FORMAT_R16_UINT, 0 );
		pImmediateContext->DrawIndexed( _geometry.numberOfIndices, 0, 0 );
	}
	
	Material GetMaterial() const { return _material; }
	Geometry GetGeometryData() const { return _geometry; }

	void SetTextureRV( ID3D11ShaderResourceView* textureRV ) { _textureRV = textureRV; }
	ID3D11ShaderResourceView* GetTextureRV() const { return _textureRV; }
	bool HasTexture() const { return _textureRV ? true : false; }
protected:
	Geometry _geometry;
	Material _material;
	ID3D11ShaderResourceView* _textureRV;
};

#endif