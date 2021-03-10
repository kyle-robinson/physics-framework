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
		// initialize vertex buffers
		HRESULT hr = vb_cube.Initialize( device.Get(), cubeVertices, ARRAYSIZE( cubeVertices ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex buffer!" );
        hr = ib_cube.Initialize( device.Get(), cubeIndices, ARRAYSIZE( cubeIndices ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create cube index buffer!" );

		// initialize index buffers
		hr = vb_plane.Initialize( device.Get(), planeVertices, ARRAYSIZE( planeVertices ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create plane vertex buffer!" );
        hr = ib_plane.Initialize( device.Get(), planeIndices, ARRAYSIZE( planeIndices ) );
        COM_ERROR_IF_FAILED( hr, "Failed to create plane index buffer!" );

		// initialize textures
		hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\sand.dds", nullptr, textureSand.GetAddressOf() );
		hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\lava.dds", nullptr, textureLava.GetAddressOf() );
		hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\beach.dds", nullptr, textureBeach.GetAddressOf() );
		hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\floor.dds", nullptr, textureGround.GetAddressOf() );
		hr = CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\marble_brown.dds", nullptr, textureMarble.GetAddressOf() );
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
	MeshData torusData = OBJLoader::Load( "Resources\\Models\\donut.obj", device.Get(), false );
	herculesGeometry.indexBuffer = torusData.IndexBuffer;
	herculesGeometry.numberOfIndices = torusData.IndexCount;
	herculesGeometry.vertexBuffer = torusData.VertexBuffer;
	herculesGeometry.vertexBufferOffset = torusData.VBOffset;
	herculesGeometry.vertexBufferStride = torusData.VBStride;

	Geometry sphereGeometry;
	MeshData sphereData = OBJLoader::Load( "Resources\\Models\\sphere.obj", device.Get(), false );
	sphereGeometry.indexBuffer = sphereData.IndexBuffer;
	sphereGeometry.numberOfIndices = sphereData.IndexCount;
	sphereGeometry.vertexBuffer = sphereData.VertexBuffer;
	sphereGeometry.vertexBufferOffset = sphereData.VBOffset;
	sphereGeometry.vertexBufferStride = sphereData.VBStride;
	
	Geometry cubeGeometry;
	cubeGeometry.vertexBufferOffset = 0;
	cubeGeometry.indexBuffer = ib_cube.Get();
	cubeGeometry.vertexBuffer = vb_cube.Get();
	cubeGeometry.vertexBufferStride = sizeof( SimpleVertex );
	cubeGeometry.numberOfIndices = ib_cube.IndexCount() * sizeof( WORD );

	Geometry planeGeometry;
	planeGeometry.vertexBufferOffset = 0;
	planeGeometry.indexBuffer = ib_plane.Get();
	planeGeometry.vertexBuffer = vb_plane.Get();
	planeGeometry.vertexBufferStride = sizeof( SimpleVertex );
	planeGeometry.numberOfIndices = ib_plane.IndexCount() * sizeof( WORD );

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
		cubes[i]->GetAppearance()->SetTextureRV( textureMarble.Get() );
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

	// initialize skysphere
	skysphere = std::make_unique<GameObject>( "Skysphere" );
	skysphere->GetTransform()->SetScale( 200.0f, 200.0f, 200.0f );
	skysphere->GetAppearance()->SetTextureRV( textureBeach.Get() );
	skysphere->GetAppearance()->SetGeometryData( sphereGeometry );
	skysphere->GetAppearance()->SetMaterial( noSpecMaterial );

	// setup particle system
	particles.resize( PARTICLE_COUNT );
	for ( uint32_t i = 0; i < PARTICLE_COUNT; i++ )
	{
		particles[i] = std::make_unique<Particle>( "Particle " + std::to_string( i + 1 ) );
		particles[i]->GetAppearance()->SetTextureRV( textureLava.Get() );
		particles[i]->GetAppearance()->SetGeometryData( cubeGeometry );
		particles[i]->GetAppearance()->SetMaterial( shinyMaterial );
	}

	// initialize physics objects
	physicsCube = std::make_unique<GameObject>( "PhysicsCube" );
	physicsCube->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
	physicsCube->GetTransform()->SetInitialPosition( 0.0f, 5.5f, 3.5f );
	physicsCube->GetAppearance()->SetTextureRV( textureMarble.Get() );
	physicsCube->GetAppearance()->SetGeometryData( cubeGeometry );
	physicsCube->GetAppearance()->SetMaterial( shinyMaterial );

	physicsCube->GetRigidBody()->SetMass( 5.0f );
	physicsCube->GetRigidBody()->SetOrientation( 1.0f, 0.0f, 0.0f, 0.0f );
	physicsCube->GetRigidBody()->SetCanSleep( true );
	physicsCube->GetRigidBody()->SetAwake( false );
	physicsCube->GetRigidBody()->SetAngularDamping( 0.8f );
	physicsCube->GetRigidBody()->SetLinearDamping( 0.95f );
	physicsCube->GetRigidBody()->SetVelocity( 0.0f, 0.0f, 0.0f );
	physicsCube->GetRigidBody()-> SetAcceleration( 0.0f, -10.0f, 0.0f );

	Matrix3 tensor;

	float coeff = 0.4 * physicsCube->GetRigidBody()->GetMass() * 1.0 * 1.0;
	tensor.SetInertiaTensorCoeffs( coeff, coeff, coeff );
	tensor.SetBlockInertiaTensor( v3df( 1.0, 1.0, 1.0 ), 5.0 );
	physicsCube->GetRigidBody()->SetInertiaTensor( tensor );

	physicsCube->GetRigidBody()->ResetForces();
	physicsCube->GetRigidBody()->CalculateDerivedData();

	return true;
}

void Graphics::CollisionResolution( std::unique_ptr<GameObject>& cube1, std::unique_ptr<GameObject>& cube2, float dt )
{
	float velocityOne = max( cube1->GetParticleModel()->GetNetForce().magnitude(), 1.0f );
	float velocityTwo = max( cube2->GetParticleModel()->GetNetForce().magnitude(), 1.0f );

	float forceMagnitude = (
		cube1->GetParticleModel()->GetMass() * velocityOne +
		cube2->GetParticleModel()->GetMass() * velocityTwo
	) / dt;

	v3df force = (
		cube2->GetTransform()->GetPosition() -
		cube1->GetTransform()->GetPosition()
	).normalize() * forceMagnitude * 0.0015f;

	v3df inverseForce;
	inverseForce.x = -force.x;
	inverseForce.y = -force.y;
	inverseForce.z = -force.z;

	cube1->GetParticleModel()->AddForce( inverseForce );
	cube2->GetParticleModel()->AddForce( force );
}

void Graphics::Update( float dt )
{
	// Update Plane Matrices
	int count = 0;
	static int tileScale = 4;
    for ( uint32_t row = 0; row < planeWidth; row++ )
    {
        for ( uint32_t col = 0; col < planeHeight; col++ )
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
	for ( uint32_t i = 0; i < cubes.size(); i++ )
		cubes[i]->Update( dt );

	physicsCube->Update( dt );

	// Check Collisions
	for ( uint32_t i = 0; i < cubes.size(); i++ )
	{
		for ( uint32_t j = 0; j < cubes.size(); j++ )
		{
			if ( i != j )
			{
				if ( useAABB )
				{
					if ( cubes[i]->GetParticleModel()->CollisionCheckAABB( cubes[j]->GetTransform()->GetPosition() ) )
						CollisionResolution( cubes[i], cubes[j], dt );
				}
				else
				{
					if ( cubes[i]->GetParticleModel()->CollisionCheckCircle(
						cubes[j]->GetTransform()->GetPosition(),
						cubes[j]->GetParticleModel()->GetCollisionRadius()
					) )
						CollisionResolution( cubes[i], cubes[j], dt );
				}
			}
		}
	}

	// Update Particles
	//for ( uint32_t i = 0; i < PARTICLE_COUNT; i++ )
	//	particles[i]->Update( dt );

	// Update Skysphere
	skysphere->Update( dt );
}

void Graphics::Draw()
{
	static float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
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
	for ( uint32_t i = 0; i < cubes.size(); i++ )
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
			context->PSSetShaderResources( 0, 1, cubes[i]->GetAppearance()->GetTextureRV() );
			cb_vs_matrix.data.HasTexture = 1.0f;
		}
		else
		{
			cb_vs_matrix.data.HasTexture = 0.0f;
		}

		if ( !cb_vs_matrix.ApplyChanges() ) return;
		cubes[i]->Draw( context.Get() );
	}

	cb_vs_matrix.data.World = XMMatrixTranspose( physicsCube->GetTransform()->GetWorldMatrix() );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	physicsCube->Draw( context.Get() );

	// Render Instanced Plane
	cb_vs_matrix.data.UseLighting = 1.0f;
	for ( uint32_t i = 0; i < planeMatrices.size(); i++ )
	{
		cb_vs_matrix.data.World = XMMatrixTranspose( XMLoadFloat4x4( &planeMatrices[i] ) );
		context->PSSetShaderResources( 0, 1, ground->GetAppearance()->GetTextureRV() );
		if ( !cb_vs_matrix.ApplyChanges() ) return;
		ground->Draw( context.Get() );
	}

	// Render Particles
	for ( uint32_t i = 0; i < PARTICLE_COUNT; i++ )
	{
		cb_vs_matrix.data.World = XMMatrixTranspose( particles[i]->GetTransform()->GetWorldMatrix() );
		context->PSSetShaderResources( 0, 1, particles[i]->GetAppearance()->GetTextureRV() );
		if ( !cb_vs_matrix.ApplyChanges() ) return;
		particles[i]->Draw( context.Get() );
	}

	// Render Torus
	cb_vs_matrix.data.UseLighting = 0.0f;
	cb_vs_matrix.data.World = XMMatrixTranspose( torus->GetTransform()->GetWorldMatrix() );
	context->PSSetShaderResources( 0, 1, torus->GetAppearance()->GetTextureRV() );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	torus->Draw( context.Get() );

	// Render Cubemap
	rasterizerStates["Cubemap"]->Bind( *this );
	skysphere->GetTransform()->SetPosition( camera->GetPositionVector3() );
	skysphere->GetTransform()->SetRotation( 0.0f, 0.0f, XM_PI );
	cb_vs_matrix.data.World = XMMatrixTranspose( skysphere->GetTransform()->GetWorldMatrix() );
	cb_vs_matrix.data.UseLighting = 1.0f;
	context->PSSetShaderResources( 0, 1, skysphere->GetAppearance()->GetTextureRV() );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	skysphere->Draw( context.Get() );

	imgui.BeginRender();
	SpawnControlWindow( cubes );
	SpawnControlWindow( particles );
	imgui.SpawnInstructionWindow();
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
	if ( ImGui::Begin( "Cube Controls", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
	{		
		ImGui::Text( "Collision Type:" );
		ImGui::SameLine();
		static int collisionGroup = 0;
		if ( ImGui::RadioButton( "AABB", &collisionGroup, 0 ) )
			useAABB = true;
		ImGui::SameLine();
		if ( ImGui::RadioButton( "Circle", &collisionGroup, 1 ) )
			useAABB = false;

		// update individual cube properties
		for ( uint32_t i = 0; i < vec.size(); i++ )
		{
			if ( ImGui::CollapsingHeader( vec[i]->GetID().c_str() ) )
			{
				float mass = vec[i]->GetParticleModel()->GetMass();
				ImGui::SliderFloat( std::string( "Mass##" ).append( std::to_string( i ) ).c_str(), &mass, 1.0f, 100.0f, "%1.f" );
				vec[i]->GetParticleModel()->SetMass( mass );

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

		if ( ImGui::CollapsingHeader( "Physics Cube" ) )
		{
			if ( ImGui::Button( "Apply Torque" ) )
			{
				/*v3df worldPos = ( physicsCube->GetTransform()->GetPosition()
					- v3df( camera->GetPositionFloat3().x, camera->GetPositionFloat3().y, camera->GetPositionFloat3().z )
					) * 0.5f;

				physicsCube->GetRigidBody()->ApplyTorque( worldPos - physicsCube->GetTransform()->GetPosition(),
					{ 0.0f, 100.0f, 0.0f } );*/

				physicsCube->GetRigidBody()->AddForceAtPoint( { 0.0f, 100.0f, 0.0f }, { 0.5f, 0.5f, 0.5f } );
			}
		}
	}
	ImGui::End();
}

void Graphics::SpawnControlWindow( std::vector<std::unique_ptr<Particle>>& vec )
{
	if ( ImGui::Begin( "Particle Controls", FALSE, ImGuiWindowFlags_NoMove ) )
	{
		float size = vec[0]->GetMaxSize();
		ImGui::SliderFloat( "Size", &size, 0.001f, 0.02f );
		ImGui::SliderFloat( "Distribution", &xDist, 1.0f, 5.0f, "%1.f" );

		for ( uint32_t i = 0; i < vec.size(); i++ )
			vec[i]->SetMaxSize( size );
	}
	ImGui::End();
}