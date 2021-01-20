#pragma once
#ifndef APPEARANCE_H
#define APPEARANCE_H

struct Material
{
	v4df diffuse;
	v4df ambient;
	v4df specular;
	float specularPower;
};

struct SurfaceInfo
{
	v4df AmbientMtrl;
	v4df DiffuseMtrl;
	v4df SpecularMtrl;
};

class Appearance
{
public:
	Material GetMaterial() const { return _material; }
	void SetTextureRV( ID3D11ShaderResourceView* textureRV ) { _textureRV = textureRV; }
	ID3D11ShaderResourceView* GetTextureRV() const { return _textureRV; }
	bool HasTexture() const { return _textureRV ? true : false; }
protected:
	Material _material;
	ID3D11ShaderResourceView* _textureRV;
};

#endif