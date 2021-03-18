#include "stdafx.h"
#include "LevelManager.h"
#include "../Graphics/Model/OBJLoader.h"
#include "../Graphics/Bindables/Rasterizer.h"
#include "../Graphics/Model/DDSTextureLoader.h"
#include "../Graphics/Objects/Data/Vertices.h"
#include "../Graphics/Objects/Data/Indices.h"
#include "../Graphics/Objects/Camera.h"
#include <imgui/imgui.h>

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
		static std::wstring path = L"Resources\\Textures\\";

		// cube textures
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"marble_brown.dds" ).c_str(), nullptr, &textures["Marble"] );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"crate.dds" ).c_str(), nullptr, &textures["Crate"] );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"compcube.dds" ).c_str(), nullptr, &textures["Companion"] );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"slime.dds" ).c_str(), nullptr, &textures["Slime"] );

		// particle textures
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"water.dds" ).c_str(), nullptr, &textures["Water"] );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"snow.dds" ).c_str(), nullptr, &textures["Snow"] );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"smoke.dds" ).c_str(), nullptr, &textures["Smoke"] );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"lava.dds" ).c_str(), nullptr, &textures["Lava"] );

		// ground textures
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"ice.dds" ).c_str(), nullptr, &textures["Ice"] );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"sand.dds" ).c_str(), nullptr, &textures["Sand"] );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"floor.dds" ).c_str(), nullptr, &textures["Tile"] );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"wood.dds" ).c_str(), nullptr, &textures["Wood"] );

		// torus textures
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"Hercules_COLOR.dds" ).c_str(), nullptr, &textures["Hercules"] );

		// skysphere textures
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"lake.dds" ).c_str(), nullptr, &textures["Lake"] );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"beach.dds" ).c_str(), nullptr, &textures["Beach"] );
		hr = CreateDDSTextureFromFile( GetDevice( gfx ), ( path + L"minecraft.dds" ).c_str(), nullptr, &textures["Minecraft"] );
		
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
	// setup camera
	camera = std::make_shared<Camera>( XMFLOAT3( 0.0f, 2.75f, -5.0f ) );
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
	ground->GetTransform()->SetScale( 50.0f, 50.0f, 0.0f );
	ground->GetTransform()->SetRotation( XMConvertToRadians( 90.0f ), 0.0f, 0.0f );
	ground->GetAppearance()->SetGeometryData( planeGeometry );
	ground->GetAppearance()->SetMaterial( noSpecMaterial );

	// initialize walls
	walls.resize( 5 );
	for ( uint32_t i = 0; i < 5; i++ )
	{
		walls[i] = std::make_unique<GameObject>( "Wall " + std::to_string( i + 1 ), false );
		walls[i]->GetTransform()->SetScale( 50.0f, 50.0f, 0.0f );
		walls[i]->GetAppearance()->SetTextureRV( textures["Tile"].Get() );
		walls[i]->GetAppearance()->SetGeometryData( planeGeometry );
		walls[i]->GetAppearance()->SetMaterial( noSpecMaterial );
		walls[i]->GetParticleModel()->IgnoreWorldCollisions();
	}
	walls[0]->GetTransform()->SetPosition( 10.0f, 0.0f, 0.0f );
	walls[1]->GetTransform()->SetPosition( -10.0f, 0.0f, 0.0f );
	walls[2]->GetTransform()->SetPosition( 0.0f, 0.0f, 20.0f );
	walls[3]->GetTransform()->SetPosition( 0.0f, 0.0f, -5.0f );
	walls[4]->GetTransform()->SetPosition( 0.0f, 10.0f, 0.0f );

	walls[0]->GetTransform()->SetRotation( 0.0f, XMConvertToRadians( 90.0f ), 0.0f );
	walls[1]->GetTransform()->SetRotation( 0.0f, XMConvertToRadians( -90.0f ), 0.0f );
	walls[2]->GetTransform()->SetRotation( 0.0f, 0.0f, 0.0f );
	walls[3]->GetTransform()->SetRotation( 0.0f, XMConvertToRadians( 180.0f ), 0.0f );
	walls[4]->GetTransform()->SetRotation( XMConvertToRadians( -90.0f ), 0.0f, 0.0f );

	// initialize torus
	torus = std::make_unique<GameObject>( "Torus", false );
	torus->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
	torus->GetTransform()->SetInitialPosition( 0.0f, 5.0f, 10.0f );
	torus->GetAppearance()->SetTextureRV( textures["Hercules"].Get() );
	torus->GetAppearance()->SetGeometryData( herculesGeometry );
	torus->GetAppearance()->SetMaterial( shinyMaterial );

	// initialize skysphere
	skysphere = std::make_unique<GameObject>( "Skysphere", false );
	skysphere->GetTransform()->SetRotation( 0.0f, 0.0f, XM_PI );
	skysphere->GetTransform()->SetScale( 200.0f, 200.0f, 200.0f );
	skysphere->GetAppearance()->SetGeometryData( sphereGeometry );
	skysphere->GetAppearance()->SetMaterial( noSpecMaterial );
}

void LevelManager::Update( Mouse& mouse, Keyboard& keyboard, float dt )
{		
	// update plane
	ground->Update( dt, false );

	// update walls
	if ( activeScene == APERATURE && updateWalls )
	{
		for ( uint32_t i = 0; i < 5; i++ )
			walls[i]->Update( dt, false );
		updateWalls = false;
	}

	// update torus
	static float rotation = 0.0f;
	rotation += dt;
	torus->GetTransform()->SetRotation( XMConvertToRadians( rotation ), 0.0f, 0.0f );
	torus->Update( dt, false );

	// update skysphere
	skysphere->GetTransform()->SetPosition( camera->GetPositionVector3() );
	skysphere->Update( dt, false );
}

void LevelManager::UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt )
{
	// update camera orientation
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

	// x world collisions
	if ( camera->GetPositionFloat3().x <= -9.9f )
		camera->SetPosition( -9.9f, camera->GetPositionFloat3().y, camera->GetPositionFloat3().z );
	if ( camera->GetPositionFloat3().x >= 9.9f )
		camera->SetPosition( 9.9f, camera->GetPositionFloat3().y, camera->GetPositionFloat3().z );

	// z world collisions
	if ( camera->GetPositionFloat3().z <= -4.9f )
		camera->SetPosition( camera->GetPositionFloat3().x, camera->GetPositionFloat3().y, -4.9f );
	if ( camera->GetPositionFloat3().z >= 19.9f )
		camera->SetPosition( camera->GetPositionFloat3().x, camera->GetPositionFloat3().y, 19.9f );
}

void LevelManager::BeginRender( Graphics& gfx )
{
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

	// update textures
	switch ( activeScene )
	{
	case SUMMER:
		ground->GetAppearance()->SetTextureRV( textures["Sand"].Get() );
		skysphere->GetAppearance()->SetTextureRV( textures["Beach"].Get() );
		break;
	case WINTER:
		ground->GetAppearance()->SetTextureRV( textures["Ice"].Get() );
		skysphere->GetAppearance()->SetTextureRV( textures["Lake"].Get() );
		break;
	case APERATURE:
		ground->GetAppearance()->SetTextureRV( textures["Tile"].Get() );
		break;
	case MINECRAFT:
		ground->GetAppearance()->SetTextureRV( textures["Wood"].Get() );
		skysphere->GetAppearance()->SetTextureRV( textures["Minecraft"].Get() );
		break;
	}

	// render plane
	cb_vs_matrix.data.UseLighting = 1.0f;
	GetContext( gfx )->PSSetShaderResources( 0, 1, ground->GetAppearance()->GetTextureRV() );
	if ( activeScene == APERATURE )
	{
		for ( uint32_t i = 0; i < 5; i++ )
		{
			cb_vs_matrix.data.World = XMMatrixTranspose( walls[i]->GetTransform()->GetWorldMatrix() );
			if ( !cb_vs_matrix.ApplyChanges() ) return;
			walls[i]->Draw( GetContext( gfx ) );
		}
	}
	cb_vs_matrix.data.World = XMMatrixTranspose( ground->GetTransform()->GetWorldMatrix() );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	ground->Draw( GetContext( gfx ) );

	// render torus
	cb_vs_matrix.data.UseLighting = 0.0f;
	cb_vs_matrix.data.World = XMMatrixTranspose( torus->GetTransform()->GetWorldMatrix() );
	GetContext( gfx )->PSSetShaderResources( 0, 1, torus->GetAppearance()->GetTextureRV() );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	torus->Draw( GetContext( gfx ) );

	// render skysphere
	cb_vs_matrix.data.UseLighting = 1.0f;
	gfx.GetRasterizer( "Cubemap" )->Bind( gfx );	
	cb_vs_matrix.data.World = XMMatrixTranspose( skysphere->GetTransform()->GetWorldMatrix() );
	GetContext( gfx )->PSSetShaderResources( 0, 1, skysphere->GetAppearance()->GetTextureRV() );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	skysphere->Draw( GetContext( gfx ) );

	SpawnControlWindow();
}

void LevelManager::SpawnControlWindow()
{
	if ( ImGui::Begin( "Theme" ), FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove )
	{
		if ( ImGui::Button( "Summer", ImVec2( ImGui::GetWindowSize().x, 0.0f ) ) ) activeScene = SUMMER;
		if ( ImGui::Button( "Winter", ImVec2( ImGui::GetWindowSize().x, 0.0f ) ) ) activeScene = WINTER;
		if ( ImGui::Button( "Aperature", ImVec2( ImGui::GetWindowSize().x, 0.0f ) ) ) activeScene = APERATURE;
		if ( ImGui::Button( "Minecraft", ImVec2( ImGui::GetWindowSize().x, 0.0f ) ) ) activeScene = MINECRAFT;
	}
	ImGui::End();
}