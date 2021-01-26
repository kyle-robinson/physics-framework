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
	Material GetMaterial() const { return _material; }
	void SetMaterial( Material material ) { _material = material; }

	Geometry GetGeometryData() const { return _geometry; }
	void SetGeometryData( Geometry geometry ) { _geometry = geometry; }

	void SetTextureRV( ID3D11ShaderResourceView* textureRV ) { _textureRV = textureRV; }
	ID3D11ShaderResourceView* GetTextureRV() const { return _textureRV; }
	bool HasTexture() const { return _textureRV ? true : false; }
public:
	Geometry _geometry;
	Material _material;
	ID3D11ShaderResourceView* _textureRV;
};

#endif