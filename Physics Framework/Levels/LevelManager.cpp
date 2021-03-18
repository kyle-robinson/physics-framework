#include "stdafx.h"
#include "LevelManager.h"
#include "../Graphics/Model/OBJLoader.h"
#include "../Graphics/Bindables/Rasterizer.h"
#include "../Graphics/Model/DDSTextureLoader.h"
#include "../Graphics/Objects/Data/Vertices.h"
#include "../Graphics/Objects/Data/Indices.h"
#include "../Graphics/Objects/Camera.h"

void LevelManager::Initialize( Graphics& gfx )
{
	InitializeMaterials( gfx );
	InitializeBuffers( gfx );
	InitializeGeometry( gfx );
	InitializeTextures( gfx );
	InitializeObjects( gfx );
}

void LevelManager::InitializeMaterials( Graphics& gfx )
{
	shinyMaterial.ambient = { 0.3f, 0.3f, 0.3f, 1.0f };
	shinyMaterial.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	shinyMaterial.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	shinyMaterial.specularPower = 10.0f;

	noSpecMaterial.ambient = { 0.1f, 0.1f, 0.1f, 1.0f };
	noSpecMaterial.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	noSpecMaterial.specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	noSpecMaterial.specularPower = 0.0f;
}

void LevelManager::InitializeGeometry( Graphics& gfx )
{
	MeshData torusData = OBJLoader::Load( "Resources\\Models\\donut.obj", GetDevice( gfx ), false );
	herculesGeometry.indexBuffer = torusData.IndexBuffer;
	herculesGeometry.numberOfIndices = torusData.IndexCount;
	herculesGeometry.vertexBuffer = torusData.VertexBuffer;
	herculesGeometry.vertexBufferOffset = torusData.VBOffset;
	herculesGeometry.vertexBufferStride = torusData.VBStride;

	MeshData sphereData = OBJLoader::Load( "Resources\\Models\\sphere.obj", GetDevice( gfx ), false );
	sphereGeometry.indexBuffer = sphereData.IndexBuffer;
	sphereGeometry.numberOfIndices = sphereData.IndexCount;
	sphereGeometry.vertexBuffer = sphereData.VertexBuffer;
	sphereGeometry.vertexBufferOffset = sphereData.VBOffset;
	sphereGeometry.vertexBufferStride = sphereData.VBStride;

	cubeGeometry.vertexBufferOffset = 0;
	cubeGeometry.indexBuffer = ib_cube.Get();
	cubeGeometry.vertexBuffer = vb_cube.Get();
	cubeGeometry.vertexBufferStride = sizeof( SimpleVertex );
	cubeGeometry.numberOfIndices = ib_cube.IndexCount() * sizeof( WORD );

	planeGeometry.vertexBufferOffset = 0;
	planeGeometry.indexBuffer = ib_plane.Get();
	planeGeometry.vertexBuffer = vb_plane.Get();
	planeGeometry.vertexBufferStride = sizeof( SimpleVertex );
	planeGeometry.numberOfIndices = ib_plane.IndexCount() * sizeof( WORD );
}

bool LevelManager::InitializeTextures( Graphics& gfx )
{
	try
	{
		HRESULT hr;
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), L"Resources\\Textures\\sand.dds", nullptr, textureSand.GetAddressOf() );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), L"Resources\\Textures\\lava.dds", nullptr, textureLava.GetAddressOf() );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), L"Resources\\Textures\\beach.dds", nullptr, textureBeach.GetAddressOf() );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), L"Resources\\Textures\\floor.dds", nullptr, textureGround.GetAddressOf() );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), L"Resources\\Textures\\marble_brown.dds", nullptr, textureMarble.GetAddressOf() );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), L"Resources\\Textures\\Hercules_COLOR.dds", nullptr, textureHercules.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to load textures in from file!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

	return true;
}

bool LevelManager::InitializeBuffers( Graphics& gfx )
{
	try
	{
		// initialize constant buffers
		HRESULT hr = cb_vs_matrix.Initialize( GetDevice( gfx ), GetContext( gfx ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create constant buffer!" );

		// initialize vertex buffers
		hr = vb_cube.Initialize( GetDevice( gfx ), cubeVertices, ARRAYSIZE( cubeVertices ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create cube vertex buffer!" );
		hr = ib_cube.Initialize( GetDevice( gfx ), cubeIndices, ARRAYSIZE( cubeIndices ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create cube index buffer!" );

		// initialize index buffers
		hr = vb_plane.Initialize( GetDevice( gfx ), planeVertices, ARRAYSIZE( planeVertices ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create plane vertex buffer!" );
		hr = ib_plane.Initialize( GetDevice( gfx ), planeIndices, ARRAYSIZE( planeIndices ) );
		COM_ERROR_IF_FAILED( hr, "Failed to create plane index buffer!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

	return true;
}

void LevelManager::InitializeObjects( Graphics& gfx )
{
	// setup plane matrices
	planeMatrices.reserve( planeWidth * planeHeight );
	for ( uint32_t i = 0; i < planeWidth * planeHeight; i++ )
	{
		XMFLOAT4X4 worldMatrix;
		XMStoreFloat4x4( &worldMatrix, XMMatrixIdentity() );
		planeMatrices.push_back( std::move( worldMatrix ) );
	}
	
	// setup camera
	camera = std::make_shared<Camera>( XMFLOAT3( 0.0f, 2.0f, -10.0f ) );
	camera->SetProjectionValues( 50.0f, static_cast< float >( gfx.GetWidth() ) /
		static_cast< float >( gfx.GetHeight() ), 0.01f, 500.0f );

	// setup the scene's light
	basicLight.AmbientLight = { 0.5f, 0.5f, 0.5f, 1.0f };
	basicLight.DiffuseLight = { 1.0f, 1.0f, 1.0f, 1.0f };
	basicLight.SpecularLight = { 0.8f, 0.8f, 0.8f, 1.0f };
	basicLight.SpecularPower = 20.0f;
	basicLight.LightVecW = { 0.0f, 1.0f, -1.0f };
	
	// initialize ground
	ground = std::make_unique<GameObject>( "Ground", false );
	ground->GetAppearance()->SetTextureRV( textureSand.Get() );
	ground->GetAppearance()->SetGeometryData( planeGeometry );
	ground->GetAppearance()->SetMaterial( noSpecMaterial );

	// initialize torus
	torus = std::make_unique<GameObject>( "Torus", false );
	torus->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
	torus->GetTransform()->SetInitialPosition( 0.0f, 5.0f, 10.0f );
	torus->GetAppearance()->SetTextureRV( textureHercules.Get() );
	torus->GetAppearance()->SetGeometryData( herculesGeometry );
	torus->GetAppearance()->SetMaterial( shinyMaterial );

	// initialize skysphere
	skysphere = std::make_unique<GameObject>( "Skysphere", false );
	skysphere->GetTransform()->SetScale( 200.0f, 200.0f, 200.0f );
	skysphere->GetAppearance()->SetTextureRV( textureBeach.Get() );
	skysphere->GetAppearance()->SetGeometryData( sphereGeometry );
	skysphere->GetAppearance()->SetMaterial( noSpecMaterial );
}

void LevelManager::Update( Mouse& mouse, Keyboard& keyboard, float dt )
{
	// update plane
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
	ground->Update( dt, false );

	// update torus
	static float rotation = 0.0f;
	rotation += dt;
	torus->GetTransform()->SetRotation( XMConvertToRadians( rotation ), 0.0f, 0.0f );
	torus->Update( dt, false );

	// update skysphere
	skysphere->Update( dt, false );
}

void LevelManager::UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt )
{
	while ( !mouse.EventBufferIsEmpty() )
	{
		Mouse::MouseEvent me = mouse.ReadEvent();
		if ( mouse.IsRightDown() )
		{
			if ( me.GetType() == Mouse::MouseEvent::EventType::RawMove )
			{
				camera->AdjustRotation(
					static_cast< float >( me.GetPosY() ) * 0.005f,
					static_cast< float >( me.GetPosX() ) * 0.005f,
					0.0f
				);
			}
		}
	}

	// camera movement
	camera->SetCameraSpeed( 0.05f );
	if ( keyboard.KeyIsPressed( VK_SHIFT ) ) camera->UpdateCameraSpeed( 0.05f );
	if ( keyboard.KeyIsPressed( 'W' ) ) Camera::MoveForward( camera, dt );
	if ( keyboard.KeyIsPressed( 'A' ) ) Camera::MoveLeft( camera, dt );
	if ( keyboard.KeyIsPressed( 'S' ) ) Camera::MoveBackward( camera, dt );
	if ( keyboard.KeyIsPressed( 'D' ) ) Camera::MoveRight( camera, dt );
	if ( keyboard.KeyIsPressed( VK_SPACE ) ) Camera::MoveUp( camera, dt );
	if ( keyboard.KeyIsPressed( VK_CONTROL ) ) Camera::MoveDown( camera, dt );

	// x world collisions
	if ( camera->GetPositionFloat3().x <= -15.0f )
		camera->SetPosition( -15.0f, camera->GetPositionFloat3().y, camera->GetPositionFloat3().z );
	if ( camera->GetPositionFloat3().x >= 15.0f )
		camera->SetPosition( 15.0f, camera->GetPositionFloat3().y, camera->GetPositionFloat3().z );

	// y world collisions
	if ( camera->GetPositionFloat3().y <= 1.0f )
		camera->SetPosition( camera->GetPositionFloat3().x, 1.0f, camera->GetPositionFloat3().z );
	if ( camera->GetPositionFloat3().y >= 10.0f )
		camera->SetPosition( camera->GetPositionFloat3().x, 10.0f, camera->GetPositionFloat3().z );

	// z world collisions
	if ( camera->GetPositionFloat3().z <= -15.0f )
		camera->SetPosition( camera->GetPositionFloat3().x, camera->GetPositionFloat3().y, -15.0f );
	if ( camera->GetPositionFloat3().z >= 15.0f )
		camera->SetPosition( camera->GetPositionFloat3().x, camera->GetPositionFloat3().y, 15.0f );
}

void LevelManager::BeginRender( Graphics& gfx )
{
	gfx.BeginFrame();

	// bind constant buffer
	GetContext( gfx )->VSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );
	GetContext( gfx )->PSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );

	// setup constant buffer
	cb_vs_matrix.data.light = basicLight;
	cb_vs_matrix.data.EyePosW = camera->GetPositionVector3();
	cb_vs_matrix.data.View = XMMatrixTranspose( camera->GetViewMatrix() );
	cb_vs_matrix.data.Projection = XMMatrixTranspose( camera->GetProjectionMatrix() );
	cb_vs_matrix.data.UseLighting = 0.0f;
}

void LevelManager::EndRender( Graphics& gfx )
{
	// setup materials
	Material material = torus->GetAppearance()->GetMaterial();
	cb_vs_matrix.data.surface.AmbientMtrl = material.ambient;
	cb_vs_matrix.data.surface.DiffuseMtrl = material.diffuse;
	cb_vs_matrix.data.surface.SpecularMtrl = material.specular;

	// render instanced plane
	cb_vs_matrix.data.UseLighting = 1.0f;
	for ( uint32_t i = 0; i < planeMatrices.size(); i++ )
	{
		cb_vs_matrix.data.World = XMMatrixTranspose( XMLoadFloat4x4( &planeMatrices[i] ) );
		GetContext( gfx )->PSSetShaderResources( 0, 1, ground->GetAppearance()->GetTextureRV() );
		if ( !cb_vs_matrix.ApplyChanges() ) return;
		ground->Draw( GetContext( gfx ) );
	}

	// render torus
	cb_vs_matrix.data.UseLighting = 0.0f;
	cb_vs_matrix.data.World = XMMatrixTranspose( torus->GetTransform()->GetWorldMatrix() );
	GetContext( gfx )->PSSetShaderResources( 0, 1, torus->GetAppearance()->GetTextureRV() );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	torus->Draw( GetContext( gfx ) );

	// render cubemap
	gfx.GetRasterizer( "Cubemap" )->Bind( gfx );
	skysphere->GetTransform()->SetPosition( camera->GetPositionVector3() );
	skysphere->GetTransform()->SetRotation( 0.0f, 0.0f, XM_PI );
	cb_vs_matrix.data.World = XMMatrixTranspose( skysphere->GetTransform()->GetWorldMatrix() );
	cb_vs_matrix.data.UseLighting = 1.0f;
	GetContext( gfx )->PSSetShaderResources( 0, 1, skysphere->GetAppearance()->GetTextureRV() );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	skysphere->Draw( GetContext( gfx ) );

	gfx.EndFrame();
}