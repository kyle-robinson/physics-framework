#pragma once

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

class GameObject
{
public:
	GameObject( std::string type, Geometry geometry, Material material );

	// Setters and Getters for position/rotation/scale
	void SetPosition( v3df position ) { _position = position; }
	void SetPosition( float x, float y, float z ) { _position[0] = x; _position[1] = y; _position[2] = z; }
	v3df GetPosition() const { return _position; }

	void SetScale( v3df scale ) { _scale = scale; }
	void SetScale( float x, float y, float z ) { _scale[0] = x; _scale[1] = y; _scale[2] = z; }
	v3df GetScale() const { return _scale; }

	void SetRotation( v3df rotation ) { _rotation = rotation; }
	void SetRotation( float x, float y, float z ) { _rotation[0] = x; _rotation[1] = y; _rotation[2] = z; }
	v3df GetRotation() const { return _rotation; }

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
	v3df _position;
	v3df _rotation;
	v3df _scale;

	XMFLOAT4X4 _world;
	std::string _type;
	GameObject* _parent;

	Geometry _geometry;
	Material _material;
	ID3D11ShaderResourceView * _textureRV;
};

