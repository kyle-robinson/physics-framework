#include "stdafx.h"
#include "Graphics.h"
#include "SwapChain.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Rasterizer.h"
#include "Viewport.h"
#include "Sampler.h"
#include "Vertices.h"
#include "Indices.h"
//#include "Terrain.h"
#include <imgui/imgui.h>

bool Graphics::Initialize( HWND hWnd, int width, int height )
{
	windowWidth = width;
	windowHeight = height;

	if ( !InitializeDirectX( hWnd ) ) return false;
	if ( !InitializeShaders() ) return false;
	if ( !InitializeScene() ) return false;
	imgui.Initialize( hWnd, device.Get(), context.Get() );

	planeMatrices.reserve( planeWidth * planeHeight );
	for ( int i = 0; i < planeWidth * planeHeight; i++ )
    {
        XMFLOAT4X4 worldMatrix;
        XMStoreFloat4x4( &worldMatrix, XMMatrixIdentity() );
        planeMatrices.push_back( std::move( worldMatrix ) );
    }

	return true;
}

bool Graphics::InitializeDirectX( HWND hWnd )
{
	try
	{
		swapChain = std::make_shared<Bind::SwapChain>( *this, context.GetAddressOf(), device.GetAddressOf(), hWnd );
		renderTarget = std::make_shared<Bind::RenderTarget>( *this, swapChain->GetSwapChain() );
        depthStencil = std::make_shared<Bind::DepthStencil>( *this );
		viewport = std::make_shared<Bind::Viewport>( *this );

		rasterizerStates.emplace( "Solid", std::make_shared<Bind::Rasterizer>( *this, true, false ) );
        rasterizerStates.emplace( "Cubemap", std::make_shared<Bind::Rasterizer>( *this, true, true ) );
        rasterizerStates.emplace( "Wireframe", std::make_shared<Bind::Rasterizer>( *this, false, true ) );

		samplerStates.emplace( "Anisotropic", std::make_shared<Bind::Sampler>( *this, Bind::Sampler::Type::Anisotropic ) );
		samplerStates.emplace( "Bilinear", std::make_shared<Bind::Sampler>( *this, Bind::Sampler::Type::Bilinear ) );
		samplerStates.emplace( "Point", std::make_shared<Bind::Sampler>( *this, Bind::Sampler::Type::Point ) );

		context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		HRESULT hr = cb_vs_matrix.Initialize( device.Get(), context.Get() );
		COM_ERROR_IF_FAILED( hr, "Failed to create constant buffer!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

    return true;
}

bool Graphics::InitializeShaders()
{
	try
	{	
		D3D11_INPUT_ELEMENT_DESC layout_nrm[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HRESULT hr = vertexShader.Initialize( device, L"Resources\\Shaders\\DX11 Framework.fx", layout_nrm, ARRAYSIZE( layout_nrm ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create light vertex shader!" );
	    hr = pixelShader.Initialize( device, L"Resources\\Shaders\\DX11 Framework.fx" );
		COM_ERROR_IF_FAILED( hr, "Failed to create light pixel shader!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

	return true;
}

bool Graphics::InitializeScene()
{
	try
	{
		HRESULT hr = vb_cube.Initialize( device.Get(), cubeVertices, ARRAYSIZE( cubeVertices ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex buffer!" );
        hr = ib_cube.Initialize( device.Get(), cubeIndices, ARRAYSIZE( cubeIndices ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube index buffer!" );

		hr = vb_plane.Initialize( device.Get(), planeVertices, ARRAYSIZE( planeVertices ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create plane vertex buffer!" );
        hr = ib_plane.Initialize( device.Get(), planeIndices, ARRAYSIZE( planeIndices ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create plane index buffer!" );

		hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\sky.dds", nullptr, textureSky.GetAddressOf() );
		hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\sand.dds", nullptr, textureSand.GetAddressOf() );
		hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\lava.dds", nullptr, textureLava.GetAddressOf() );
		hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\stone.dds", nullptr, textureStone.GetAddressOf() );
		hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\floor.dds", nullptr, textureGround.GetAddressOf() );
		hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\Hercules_COLOR.dds", nullptr, textureHercules.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to load textures in from file!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

	// setup camera
	camera = std::make_unique<Camera>( XMFLOAT3( 0.0f, 2.0f, -10.0f ) );
	camera->SetProjectionValues( 50.0f, static_cast<float>( windowWidth ) / static_cast<float>( windowHeight ), 0.01f, 500.0f );

	// setup terrain
	/*TerrainInfo info;
	info.filePath = L"Resources\\Terrain\\Heightmap 513x513.raw";
	info.heightMapScale = 5.0f;
	info.numRows = 100;
	info.numCols = 100;
	info.cellSpacing = 0.5f;
	terrain = std::make_shared<Terrain>();
	terrain->Initialize( *this, info );*/

	// setup the scene's light
	basicLight.AmbientLight = { 0.5f, 0.5f, 0.5f, 1.0f };
	basicLight.DiffuseLight = { 1.0f, 1.0f, 1.0f, 1.0f };
	basicLight.SpecularLight = { 0.8f, 0.8f, 0.8f, 1.0f };
	basicLight.SpecularPower = 20.0f;
	basicLight.LightVecW = { 0.0f, 1.0f, -1.0f };

	// create materials
	Material shinyMaterial;
	shinyMaterial.ambient = { 0.3f, 0.3f, 0.3f, 1.0f };
	shinyMaterial.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	shinyMaterial.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	shinyMaterial.specularPower = 10.0f;

	Material noSpecMaterial;
	noSpecMaterial.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
	noSpecMaterial.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	noSpecMaterial.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	noSpecMaterial.specularPower = 0.0f;

	// create geometry pieces
	Geometry herculesGeometry;
	objMeshData = OBJLoader::Load( "Resources\\Models\\donut.obj", device.Get() );
	herculesGeometry.indexBuffer = objMeshData.IndexBuffer;
	herculesGeometry.numberOfIndices = objMeshData.IndexCount;
	herculesGeometry.vertexBuffer = objMeshData.VertexBuffer;
	herculesGeometry.vertexBufferOffset = objMeshData.VBOffset;
	herculesGeometry.vertexBufferStride = objMeshData.VBStride;
	
	Geometry cubeGeometry;
	cubeGeometry.vertexBufferOffset = 0;
	cubeGeometry.indexBuffer = ib_cube.Get();
	cubeGeometry.vertexBuffer = vb_cube.Get();
	cubeGeometry.numberOfIndices = vb_cube.VertexCount() * sizeof( SimpleVertex );
	cubeGeometry.vertexBufferStride = sizeof( SimpleVertex );

	Geometry planeGeometry;
	planeGeometry.vertexBufferOffset = 0;
	planeGeometry.indexBuffer = ib_plane.Get();
	planeGeometry.vertexBuffer = vb_plane.Get();
	planeGeometry.numberOfIndices = vb_plane.VertexCount() * sizeof( SimpleVertex );
	planeGeometry.vertexBufferStride = sizeof( SimpleVertex );

	// initialize ground
	ground = std::make_unique<GameObject>( "Ground" );
	ground->GetAppearance()->SetTextureRV( textureSand.Get() );
	ground->GetAppearance()->SetGeometryData( planeGeometry );
	ground->GetAppearance()->SetMaterial( noSpecMaterial );

	// initialize cubes
	cubes.resize( NUMBER_OF_CUBES );
	for ( auto i = 0; i < NUMBER_OF_CUBES; i++ )
	{
		cubes[i] = std::make_unique<GameObject>( "Cube " + std::to_string( i + 1 ) );
		cubes[i]->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
		cubes[i]->GetTransform()->SetInitialPosition( -4.0f + ( i * 2.0f ), 0.5f, 10.0f );
		cubes[i]->GetAppearance()->SetTextureRV( textureStone.Get() );
		cubes[i]->GetAppearance()->SetGeometryData( cubeGeometry );
		cubes[i]->GetAppearance()->SetMaterial( shinyMaterial );
	}

	// initialize torus
	torus = std::make_unique<GameObject>( "Torus" );
	torus->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
	torus->GetTransform()->SetInitialPosition( 0.0f, 5.0f, 10.0f );
	torus->GetAppearance()->SetTextureRV( textureHercules.Get() );
	torus->GetAppearance()->SetGeometryData( herculesGeometry );
	torus->GetAppearance()->SetMaterial( shinyMaterial );

	// initialize skybox
	skybox = std::make_unique<GameObject>( "Skybox" );
	skybox->GetTransform()->SetScale( 200.0f, 200.0f, 200.0f );
	skybox->GetTransform()->SetInitialPosition( camera->GetPositionVector3() );
	skybox->GetAppearance()->SetTextureRV( textureSky.Get() );
	skybox->GetAppearance()->SetGeometryData( cubeGeometry );
	skybox->GetAppearance()->SetMaterial( noSpecMaterial );

	// setup particle system
	particles.resize( PARTICLE_COUNT );
	for ( unsigned int i = 0; i < PARTICLE_COUNT; i++ )
	{
		particles[i] = std::make_unique<Particle>( "Particle " + std::to_string( i + 1 ) );
		particles[i]->GetAppearance()->SetTextureRV( textureLava.Get() );
		particles[i]->GetAppearance()->SetGeometryData( cubeGeometry );
		particles[i]->GetAppearance()->SetMaterial( shinyMaterial );
	}

	return true;
}

void Graphics::Update( float dt )
{
	// Update Plane Matrices
	int count = 0;
	static int tileScale = 4;
    for ( unsigned int row = 0; row < planeWidth; row++ )
    {
        for ( unsigned int col = 0; col < planeHeight; col++ )
        {
            XMStoreFloat4x4( &planeMatrices[count],
				XMMatrixScaling( tileScale, tileScale, 0.0f ) *
                XMMatrixRotationX( XMConvertToRadians( 90.0f ) ) *
                XMMatrixTranslation(
					-20.0f + ( col * 2 ) * tileScale,
					0.0f,
					25.0f - ( row * 2 ) * tileScale
				)
			);
            count++;
        }
    }
	ground->Update( dt );
	
	// Update Torus
	static float rotation = 0.0f;
	rotation += dt;
	torus->GetTransform()->SetRotation( XMConvertToRadians( rotation ), 0.0f, 0.0f );
	torus->Update( dt );

	// Update Cubes
	for ( unsigned int i = 0; i < cubes.size(); i++ )
		cubes[i]->Update( dt );

	// Update Particles
	for ( unsigned int i = 0; i < PARTICLE_COUNT; i++ )
		particles[i]->Update( dt );

	// Update Skybox
	skybox->Update( dt );
}

void Graphics::Draw()
{
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	renderTarget->BindAsBuffer( *this, depthStencil.get(), clearColor );
    depthStencil->ClearDepthStencil( *this );
	rasterizerStates["Solid"]->Bind( *this );
	samplerStates["Anisotropic"]->Bind( *this );

    // Setup Buffers
	Shaders::BindShaders( context.Get(), vertexShader, pixelShader );
	context->VSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );
	context->PSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );

	// Setup Constant Buffer
	cb_vs_matrix.data.light = basicLight;
	cb_vs_matrix.data.EyePosW = camera->GetPositionVector3();
	cb_vs_matrix.data.View = XMMatrixTranspose( camera->GetViewMatrix() );
	cb_vs_matrix.data.Projection = XMMatrixTranspose( camera->GetProjectionMatrix() );
	cb_vs_matrix.data.UseLighting = 0.0f;

	// Render Scene Objects
	ID3D11ShaderResourceView* textureToUse;
	for ( unsigned int i = 0; i < cubes.size(); i++ )
	{
		// Get Materials
		Material material = cubes[i]->GetAppearance()->GetMaterial();
		cb_vs_matrix.data.surface.AmbientMtrl = material.ambient;
		cb_vs_matrix.data.surface.DiffuseMtrl = material.diffuse;
		cb_vs_matrix.data.surface.SpecularMtrl = material.specular;
		cb_vs_matrix.data.World = XMMatrixTranspose( cubes[i]->GetTransform()->GetWorldMatrix() );

		// Set Textures
		if ( cubes[i]->GetAppearance()->HasTexture() )
		{
			textureToUse = cubes[i]->GetAppearance()->GetTextureRV();
			context->PSSetShaderResources( 0, 1, &textureToUse );
			cb_vs_matrix.data.HasTexture = 1.0f;
		}
		else
		{
			cb_vs_matrix.data.HasTexture = 0.0f;
		}

		if ( !cb_vs_matrix.ApplyChanges() ) return;
		cubes[i]->Draw( context.Get() );
	}

	// Draw Terrain
	cb_vs_matrix.data.UseLighting = 1.0f;
	//terrain->Draw( *this, cb_vs_matrix, XMMatrixIdentity() );

	// Render Instanced Plane
	for ( unsigned int i = 0; i < planeMatrices.size(); i++ )
	{
		cb_vs_matrix.data.World = XMMatrixTranspose( XMLoadFloat4x4( &planeMatrices[i] ) );
		textureToUse = ground->GetAppearance()->GetTextureRV();
		context->PSSetShaderResources( 0, 1, &textureToUse );
		if ( !cb_vs_matrix.ApplyChanges() ) return;
		ground->Draw( context.Get() );
	}

	// Render Particles
	for ( unsigned int i = 0; i < PARTICLE_COUNT; i++ )
	{
		cb_vs_matrix.data.World = XMMatrixTranspose( particles[i]->GetTransform()->GetWorldMatrix() );
		textureToUse = particles[i]->GetAppearance()->GetTextureRV();
		context->PSSetShaderResources( 0, 1, &textureToUse );
		if ( !cb_vs_matrix.ApplyChanges() ) return;
		particles[i]->Draw( context.Get() );
	}

	// Render Torus
	cb_vs_matrix.data.UseLighting = 0.0f;
	cb_vs_matrix.data.World = XMMatrixTranspose( torus->GetTransform()->GetWorldMatrix() );
	textureToUse = torus->GetAppearance()->GetTextureRV();
	context->PSSetShaderResources( 0, 1, &textureToUse );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	torus->Draw( context.Get() );

	// Render Cubemap
	rasterizerStates["Cubemap"]->Bind( *this );
	skybox->GetTransform()->SetPosition( camera->GetPositionVector3() );
	cb_vs_matrix.data.World = XMMatrixTranspose( skybox->GetTransform()->GetWorldMatrix() );
	cb_vs_matrix.data.UseLighting = 1.0f;
	textureToUse = skybox->GetAppearance()->GetTextureRV();
	context->PSSetShaderResources( 0, 1, &textureToUse );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	skybox->Draw( context.Get() );

	imgui.BeginRender();
	SpawnControlWindow( cubes );
	SpawnControlWindow( particles );
	imgui.EndRender();

    // display frame
	HRESULT hr = swapChain->GetSwapChain()->Present( 1, NULL );
	if ( FAILED( hr ) )
	{
		hr == DXGI_ERROR_DEVICE_REMOVED ?
            ErrorLogger::Log( device->GetDeviceRemovedReason(), "Swap Chain. Graphics device removed!" ) :
            ErrorLogger::Log( hr, "Swap Chain failed to render frame!" );
		exit( -1 );
	}
}

// IMGUI WINDOWS //
void Graphics::SpawnControlWindow( std::vector<std::unique_ptr<GameObject>>& vec )
{
	if ( ImGui::Begin( "Cube Controls", FALSE, ImGuiWindowFlags_NoMove ) )
	{
		for ( unsigned int i = 0; i < vec.size(); i++ )
		{
			if ( ImGui::CollapsingHeader( vec[i]->GetID().c_str() ) )
			{
				float friction = vec[i]->GetParticleModel()->GetFriction();
				ImGui::SliderFloat( std::string( "Friction##" ).append( std::to_string( i ) ).c_str(), &friction, 0.0f, 0.0008f, "%.7f", 10 );
				vec[i]->GetParticleModel()->SetFriction( friction );

				float dragFactor = vec[i]->GetParticleModel()->GetDragFactor();
				ImGui::SliderFloat( std::string( "Drag Factor##" ).append( std::to_string( i ) ).c_str(), &dragFactor, 0.0f, 10.0f, "%1.f" );
				vec[i]->GetParticleModel()->SetDragFactor( dragFactor );

				ImGui::Text( "Drag Type: " );
				ImGui::SameLine();
				static int dragFactorGroup = 0;
				if ( ImGui::RadioButton( std::string( "Laminar##" ).append( std::to_string( i ) ).c_str(), &dragFactorGroup + i, 0 ) )
					vec[i]->GetParticleModel()->SetLaminar( true );
				ImGui::SameLine();
				if ( ImGui::RadioButton( std::string( "Turbulent##" ).append( std::to_string( i ) ).c_str(), &dragFactorGroup + i, 1 ) )
					vec[i]->GetParticleModel()->SetLaminar( false );
			}
		}
	} ImGui::End();
}

void Graphics::SpawnControlWindow( std::vector<std::unique_ptr<Particle>>& vec )
{
	if ( ImGui::Begin( "Particle Controls", FALSE, ImGuiWindowFlags_NoMove ) )
	{
		float size = vec[0]->GetMaxSize();
		ImGui::SliderFloat( "Size", &size, 0.001f, 0.02f );
		ImGui::SliderFloat( "Distribution", &xDist, 1.0f, 5.0f, "%1.f" );

		for ( unsigned int i = 0; i < vec.size(); i++ )
			vec[i]->SetMaxSize( size );
	} ImGui::End();
}