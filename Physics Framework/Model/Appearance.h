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
	int numberOfVertices;
	int numberOfIndices;
	UINT vertexBufferStride;
	UINT vertexBufferOffset;
	std::vector<XMFLOAT3> vertices;
	std::vector<unsigned short> indices;
};

class Appearance
{
public:
	Appearance() : _geometry( {} ), _material( {} ), _textureRV( nullptr ) {}

	void Update( ID3D11DeviceContext* context )
	{
		context->IASetVertexBuffers( 0, 1, &_geometry.vertexBuffer,
			&_geometry.vertexBufferStride, &_geometry.vertexBufferOffset );
		context->IASetIndexBuffer( _geometry.indexBuffer, DXGI_FORMAT_R16_UINT, 0 );
		context->DrawIndexed( _geometry.numberOfIndices, 0, 0 );
	}

	Material GetMaterial() const { return _material; }
	void SetMaterial( Material material ) { _material = material; }

	Geometry GetGeometryData() const { return _geometry; }
	void SetGeometryData( Geometry geometry ) { _geometry = geometry; }

	void SetTextureRV( ID3D11ShaderResourceView* textureRV ) { _textureRV = textureRV; }
	ID3D11ShaderResourceView* GetTextureRV() const { return _textureRV; }
	bool HasTexture() const { return _textureRV ? true : false; }
public:
	Geometry _geometry;
private:
	Material _material;
	ID3D11ShaderResourceView* _textureRV;
};

#endif