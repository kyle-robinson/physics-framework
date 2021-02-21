#pragma once
#ifndef VERTEX_H
#define VERTEX_H

#include <string>
#include <DirectXMath.h>

struct SimpleVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexCoord;

	bool operator < ( const SimpleVertex other ) const
	{
		return std::memcmp( ( void* )this, ( void* )&other, sizeof( SimpleVertex ) ) > 0;
	};
};

struct MeshData
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT VBStride;
	UINT VBOffset;
	UINT IndexCount;
	UINT VertexCount;
	std::vector<XMFLOAT3> vertices;
	std::vector<unsigned short> indices;
};

#endif