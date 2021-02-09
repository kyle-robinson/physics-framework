#pragma once
#ifndef TERRAIN_H
#define TERRAIN_H

#include "GraphicsResource.h"
#include "ConstantBuffer.h"
#include "Camera.h"

struct TerrainInfo
{
	std::wstring filePath;
	float heightMapScale;
	UINT numRows;
	UINT numCols;
	float cellSpacing;
};

class Terrain : public GraphicsResource
{
public:
	Terrain();
	void Initialize( Graphics& gfx, const TerrainInfo& info );
	const float GetWidth() const noexcept { return (terrainInfo.numCols - 1) * terrainInfo.cellSpacing; };
	const float GetDepth() const noexcept { return (terrainInfo.numRows - 1) * terrainInfo.cellSpacing; };
	const float GetHeight( float x, float z ) const noexcept;
	void Draw( Graphics& gfx, ConstantBuffer<CB_VS_matrix>& cb_vs_matrix, XMMATRIX worldMatrix );
private:
	void LoadHeightMap();
	void BuildVertexBuffer( Graphics& gfx );
	void BuildIndexBuffer( Graphics& gfx );
private:
	UINT numFaces;
	UINT numVertices;
	TerrainInfo terrainInfo;
	std::vector<float> heightMap;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
};

#endif