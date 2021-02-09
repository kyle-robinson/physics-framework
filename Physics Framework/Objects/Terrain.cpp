#include "stdafx.h"
#include "Terrain.h"

Terrain::Terrain()
{
    vertexBuffer = nullptr;
    indexBuffer = nullptr;
    terrainInfo = {};
}

void Terrain::Initialize( Graphics& gfx, const TerrainInfo& info )
{
    terrainInfo = info;
    numVertices = terrainInfo.numRows * terrainInfo.numCols;
    numFaces = (terrainInfo.numRows - 1) * (terrainInfo.numCols - 1) * 2;
    LoadHeightMap();
    BuildVertexBuffer( gfx );
    BuildIndexBuffer( gfx );
}

const float Terrain::GetHeight( float x, float z ) const noexcept
{
    // Transform from terrain local space to "cell" space.
    float c = ( x + 0.5f * GetWidth() ) /  terrainInfo.cellSpacing;
    float d = ( z - 0.5f * GetDepth() ) / -terrainInfo.cellSpacing;

    // Get the row and column we are in.
    int row = static_cast<int>( floorf( d ) );
    int col = static_cast<int>( floorf( c ) );

    // Grab the heights of the cell we are in.
    // A*--*B
    //  | /|
    //  |/ |
    // C*--*D
    float A = heightMap[row * terrainInfo.numCols + col];
    float B = heightMap[row * terrainInfo.numCols + col + 1];
    float C = heightMap[(row + 1) * terrainInfo.numCols + col];
    float D = heightMap[(row + 1) * terrainInfo.numCols + col + 1];

    // Where we are relative to the cell.
    float s = c - static_cast<float>( col );
    float t = d - static_cast<float>( row );

    // If upper triangle ABC.
    if ( s + t <= 1.0f )
    {
        float uy = B - A;
        float vy = C - A;
        return A + s * uy + t * vy;
    }
    else // lower triangle DCB.
    {
        float uy = C - D;
        float vy = B - D;
        return D + ( 1.0f - s ) * uy + ( 1.0f - t ) * vy;
    }
}

void Terrain::Draw( Graphics& gfx, ConstantBuffer<CB_VS_matrix>& cb_vs_matrix, XMMATRIX worldMatrix )
{
    UINT stride = sizeof( SimpleVertex );
    UINT offset = 0;
    GetContext( gfx )->IASetVertexBuffers( 0, 1, &vertexBuffer, &stride, &offset );
    GetContext( gfx )->IASetIndexBuffer( indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0 );

    cb_vs_matrix.data.World = XMMatrixTranspose( worldMatrix );
    if ( !cb_vs_matrix.ApplyChanges() ) return;
    GetContext( gfx )->DrawIndexed( numFaces * 3, 0, 0 );
}

void Terrain::LoadHeightMap()
{
	// A height for each vertex 
	std::vector<unsigned char> in( terrainInfo.numRows * terrainInfo.numCols );

	// Open the file.
	std::ifstream inFile; 
	inFile.open( terrainInfo.filePath.c_str(), std::ios_base::binary );
	if ( inFile )
	{
		// Read the RAW bytes.
		inFile.read( (char*)&in[0], (std::streamsize)in.size() );
		// Done with file.
		inFile.close(); 
	}

	// Copy the array data into a float array and scale it.
    heightMap.resize( terrainInfo.numCols * terrainInfo.numRows, 0 );
	for ( UINT i = 0; i < terrainInfo.numRows * terrainInfo.numCols; ++i ) 
	{
		heightMap[i] = ( in[i] / 255.0f ) * terrainInfo.heightMapScale;
	}
}

void Terrain::BuildVertexBuffer( Graphics& gfx )
{
    std::vector<SimpleVertex> vertices( numVertices );

    float halfWidth = ( terrainInfo.numCols - 1) * terrainInfo.cellSpacing * 0.5f;
    float halfDepth = ( terrainInfo.numRows - 1) * terrainInfo.cellSpacing * 0.5f;

    float du = 1.0f / ( terrainInfo.numCols - 1 );
    float dv = 1.0f / ( terrainInfo.numRows - 1 );
    for ( UINT i = 0; i < terrainInfo.numRows; ++i )
    {
        float z = halfDepth - i * terrainInfo.cellSpacing;
        for ( UINT j = 0; j < terrainInfo.numCols; ++j )
        {
            float x = -halfWidth + j * terrainInfo.cellSpacing;

            float y = heightMap[i * terrainInfo.numCols + j];
            vertices[i * terrainInfo.numCols + j].Position = XMFLOAT3( x, y, z );
            vertices[i * terrainInfo.numCols + j].Normal = XMFLOAT3( 0.0f, 1.0f, 0.0f );

            // Stretch texture over grid.
            vertices[i * terrainInfo.numCols + j].TexCoord.x = j * du;
            vertices[i * terrainInfo.numCols + j].TexCoord.y = i * dv;
        }
    }

    // Estimate normals for interior nodes using central difference.
    float invTwoDX = 1.0f / ( 2.0f * terrainInfo.cellSpacing );
    float invTwoDZ = 1.0f / ( 2.0f * terrainInfo.cellSpacing );
    for ( UINT i = 2; i < terrainInfo.numRows - 1; ++i )
    {
        for ( UINT j = 2; j < terrainInfo.numCols - 1; ++j )
        {
            float t = heightMap[(i - 1) * terrainInfo.numCols + j];
            float b = heightMap[(i + 1) * terrainInfo.numCols + j];
            float l = heightMap[i * terrainInfo.numCols + j - 1];
            float r = heightMap[i * terrainInfo.numCols + j + 1];

            XMFLOAT3 tanZ( 0.0f, ( t - b ) * invTwoDZ, 1.0f );
            XMFLOAT3 tanX( 1.0f, ( r - l ) * invTwoDX, 0.0f );

            XMVECTOR N = XMVector3Cross( XMLoadFloat3( &tanZ ), XMLoadFloat3( &tanX ) );
            N = XMVector3Normalize( N );
            XMStoreFloat3( &vertices[i * terrainInfo.numCols + j].Normal, N );

        }
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof( SimpleVertex ) * numVertices;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    try
    {
        HRESULT hr = GetDevice( gfx )->CreateBuffer(&vbd, &vinitData, &vertexBuffer );
        COM_ERROR_IF_FAILED( hr, "Failed to initialize terrain vertex buffer!" );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return;
    }
}

void Terrain::BuildIndexBuffer( Graphics& gfx )
{
    std::vector<DWORD> indices( numFaces * 3 ); // 3 indices per face

    // Iterate over each quad and compute indices.
    int k = 0;
    for ( UINT i = 0; i < terrainInfo.numRows - 1; ++i )
    {
        for ( UINT j = 0; j < terrainInfo.numCols - 1; ++j )
        {
            indices[k] = i * terrainInfo.numCols + j;
            indices[k + 1] = i * terrainInfo.numCols + j + 1;
            indices[k + 2] = (i + 1) * terrainInfo.numCols + j;

            indices[k + 3] = (i + 1) * terrainInfo.numCols + j;
            indices[k + 4] = i * terrainInfo.numCols + j + 1;
            indices[k + 5] = (i + 1) * terrainInfo.numCols + j + 1;

            k += 6; // next quad
        }
    }

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof( DWORD ) * numFaces * 3;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    try
    {
        HRESULT hr = GetDevice( gfx )->CreateBuffer( &ibd, &iinitData, &indexBuffer );
        COM_ERROR_IF_FAILED( hr, "Failed to initialize terrain index buffer!" );
    }
    catch ( COMException& exception )
    {
        ErrorLogger::Log( exception );
        return;
    }
}