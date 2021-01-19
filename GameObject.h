#pragma once

#include "Transform.h"

using namespace DirectX;

struct Geometry
{
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int numberOfIndices;

	UINT vertexBufferStride;
	UINT vertexBufferOffset;
};

struct Material
{
	v4df diffuse;
	v4df ambient;
	v4df specular;
	float specularPower;
};

class GameObject : public Transform
{
public:
	GameObject( std::string type, Geometry geometry, Material material );

	// Setters and Getters for position/rotation/scale
	std::string GetType() const { return _type; }
	Geometry GetGeometryData() const { return _geometry; }
	Material GetMaterial() const { return _material; }
	XMMATRIX GetWorldMatrix() const { return XMLoadFloat4x4( &_world ); }

	void SetTextureRV( ID3D11ShaderResourceView * textureRV ) { _textureRV = textureRV; }
	ID3D11ShaderResourceView* GetTextureRV() const { return _textureRV; }
	bool HasTexture() const { return _textureRV ? true : false; }
	void SetParent( GameObject* parent ) { _parent = parent; }

	void Update(float t);
	void Draw( ID3D11DeviceContext* pImmediateContext );

private:
	XMFLOAT4X4 _world;
	std::string _type;
	GameObject* _parent;

	Geometry _geometry;
	Material _material;
	ID3D11ShaderResourceView * _textureRV;
};

