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
#include <imgui/imgui.h>

bool Graphics::Initialize( HWND hWnd, int width, int height )
{
	windowWidth = width;
	windowHeight = height;

	if ( !InitializeDirectX( hWnd ) ) return false;
	if ( !InitializeShaders() ) return false;
	if ( !InitializeScene() ) return false;
	imgui.Initialize( hWnd, device.Get(), context.Get() );

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
		samplerStates["Anisotropic"]->Bind( *this );

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
	camera = std::make_unique<Camera>(
		v3df( 0.0f, 2.0f, -1.0f ), // eye
		v3df( 0.0f, 2.0f,  0.0f ), // at
		v3df( 0.0f, 1.0f,  0.0f ), // up
		static_cast<float>( windowWidth ),
		static_cast<float>( windowHeight ),
		0.01f, 300.0f
	);

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
	ground->GetTransform()->SetInitialPosition( 0.0f, 0.0f, 0.0f );
	ground->GetTransform()->SetScale( 50.0f, 50.0f, 50.0f );
	ground->GetTransform()->SetRotation( XMConvertToRadians( 90.0f ), 0.0f, 0.0f );
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
	skybox->GetTransform()->SetInitialPosition( camera->GetPosition() );
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
	camera->Update();
	ground->Update( dt );
	
	static float rotation = 0.0f;
	rotation += dt;
	torus->GetTransform()->SetRotation( XMConvertToRadians( rotation ), 0.0f, 0.0f );
	torus->Update( dt );

	for ( unsigned int i = 0; i < cubes.size(); i++ )
	{
		//cubes[i]->GetTransform()->SetRotation( 0.0f, XMConvertToRadians( rotation * 0.1f * i ), 0.0f );
		v3df worldPosition = ( cubes[i]->GetRigidBody()->GetTransform()->GetPosition() + camera->GetPosition() ) * 0.5f;
		cubes[i]->GetRigidBody()->ApplyTorque( worldPosition - cubes[i]->GetRigidBody()->GetTransform()->GetPosition(), { 0.0f, 100.0f, 0.0f } );
		cubes[i]->Update( dt );
	}

	for ( unsigned int i = 0; i < PARTICLE_COUNT; i++ )
		particles[i]->Update( dt );

	skybox->Update( dt );
}

void Graphics::Draw()
{
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	renderTarget->BindAsBuffer( *this, depthStencil.get(), clearColor );
    depthStencil->ClearDepthStencil( *this );
	rasterizerStates["Solid"]->Bind( *this );

    // Setup Buffers
	Shaders::BindShaders( context.Get(), vertexShader, pixelShader );
	context->VSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );
	context->PSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );

	// Setup Constant Buffer
	cb_vs_matrix.data.View = XMMatrixTranspose( XMLoadFloat4x4( &camera->GetView() ) );
	cb_vs_matrix.data.Projection = XMMatrixTranspose( XMLoadFloat4x4( &camera->GetProjection() ) );
	cb_vs_matrix.data.light = basicLight;
	cb_vs_matrix.data.EyePosW = camera->GetPosition();
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

	// Render Particles
	cb_vs_matrix.data.UseLighting = 1.0f;
	for ( unsigned int i = 0; i < PARTICLE_COUNT; i++ )
	{
		cb_vs_matrix.data.World = XMMatrixTranspose( particles[i]->GetTransform()->GetWorldMatrix() );
		textureToUse = particles[i]->GetAppearance()->GetTextureRV();
		context->PSSetShaderResources( 0, 1, &textureToUse );
		if ( !cb_vs_matrix.ApplyChanges() ) return;
		particles[i]->Draw( context.Get() );
	}

	// Render Other Objects
	cb_vs_matrix.data.UseLighting = 0.0f;
	cb_vs_matrix.data.World = XMMatrixTranspose( torus->GetTransform()->GetWorldMatrix() );
	textureToUse = torus->GetAppearance()->GetTextureRV();
	context->PSSetShaderResources( 0, 1, &textureToUse );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	torus->Draw( context.Get() );

	cb_vs_matrix.data.World = XMMatrixTranspose( ground->GetTransform()->GetWorldMatrix() );
	textureToUse = ground->GetAppearance()->GetTextureRV();
	context->PSSetShaderResources( 0, 1, &textureToUse );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	ground->Draw( context.Get() );

	// Render Cubemap
	rasterizerStates["Cubemap"]->Bind( *this );
	skybox->GetTransform()->SetPosition( camera->GetPosition() );
	cb_vs_matrix.data.World = XMMatrixTranspose( skybox->GetTransform()->GetWorldMatrix() );
	cb_vs_matrix.data.UseLighting = 1.0f;
	ID3D11ShaderResourceView* skyboxTexture = skybox->GetAppearance()->GetTextureRV();
	context->PSSetShaderResources( 0, 1, &skyboxTexture );
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

//---------------//
// IMGUI WINDOWS //
//---------------//
void Graphics::SpawnControlWindow( std::vector<std::unique_ptr<GameObject>>& vec )
{
	if ( ImGui::Begin( "Cube Controls", FALSE ) )
	{
		for ( unsigned int i = 0; i < vec.size(); i++ )
		{
			if ( ImGui::CollapsingHeader( vec[i]->GetID().c_str() ) )
			{
				float timeStep = vec[i]->GetParticleModel()->GetTimeStep();
				ImGui::SliderFloat( "Time Step", &timeStep, 0.1f, 1.0f, "%.1f" );
				vec[i]->GetParticleModel()->SetTimeStep( timeStep );

				float gravity = vec[i]->GetParticleModel()->GetGravity();
				ImGui::SliderFloat( "Gravity", &gravity, 4.905f, 19.62f, "%.7f" );
				vec[i]->GetParticleModel()->SetGravity( gravity );

				float friction = vec[i]->GetParticleModel()->GetFriction();
				ImGui::SliderFloat( "Friction", &friction, 0.0f, 0.0008f, "%.7f", 10 );
				vec[i]->GetParticleModel()->SetFriction( friction );

				float dragFactor = vec[i]->GetParticleModel()->GetDragFactor();
				ImGui::SliderFloat( "Drag Factor", &dragFactor, 0.0f, 10.0f, "%1.f" );
				vec[i]->GetParticleModel()->SetDragFactor( dragFactor );

				ImGui::Text( "Drag Type: " );
				ImGui::SameLine();
				static int dragFactorGroup = 0;
				if ( ImGui::RadioButton( "Laminar", &dragFactorGroup, 0 ) )
					vec[i]->GetParticleModel()->SetLaminar( true );
				ImGui::SameLine();
				if ( ImGui::RadioButton( "Turbulent", &dragFactorGroup, 1 ) )
					vec[i]->GetParticleModel()->SetLaminar( false );
			}
		}
	} ImGui::End();
}

void Graphics::SpawnControlWindow( std::vector<std::unique_ptr<Particle>>& vec )
{
	if ( ImGui::Begin( "Particle Controls", FALSE ) )
	{
		float size = vec[0]->GetMaxSize();
		ImGui::SliderFloat( "Size", &size, 0.001f, 0.02f );
		ImGui::SliderFloat( "Distribution", &xDist, 1.0f, 25.0f, "%.1f" );

		for ( unsigned int i = 0; i < vec.size(); i++ )
			vec[i]->SetMaxSize( size );
	} ImGui::End();
}