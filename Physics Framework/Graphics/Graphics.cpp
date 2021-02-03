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

/*bool Graphics::HandleKeyboard( MSG msg )
{
	switch ( msg.wParam )
	{
	// window management
	case VK_ESCAPE:
		PostQuitMessage( 0 );
		return true;

	// camera movement
	case VK_UP:
		cameraOrbitRadius = max( cameraOrbitRadiusMin, cameraOrbitRadius - ( cameraSpeed * 0.2f ) );
		return true;
	case VK_DOWN:
		cameraOrbitRadius = min( cameraOrbitRadiusMax, cameraOrbitRadius + ( cameraSpeed * 0.2f ) );
		return true;
	case VK_RIGHT:
		cameraOrbitAngleXZ -= cameraSpeed;
		return true;
	case VK_LEFT:
		cameraOrbitAngleXZ += cameraSpeed;
		return true;

	// set object to use
	case '1': objectToUse = 1; return true;
	case '2': objectToUse = 2; return true;
	case '3': objectToUse = 3; return true;
	case '4': objectToUse = 4; return true;
	case '5': objectToUse = 5; return true;
	}

	return false;
}*/

bool Graphics::Initialize( HWND hWnd, int width, int height )
{
	windowWidth = width;
	windowHeight = height;
	
	//if ( !InitializeWindow( hInstance, nCmdShow ) ) return false;
	if ( !InitializeDirectX( hWnd ) ) return false;
	if ( !InitializeShaders() ) return false;
	if ( !InitializeScene() ) return false;

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
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HRESULT hr = vertexShader.Initialize( device, L"Resources\\Shaders\\DX11 Framework.fx", layout, ARRAYSIZE( layout ) );
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

		CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\stone.dds", nullptr, textureStone.GetAddressOf() );
		CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\floor.dds", nullptr, textureGround.GetAddressOf() );
		CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\Hercules_COLOR.dds", nullptr, textureHercules.GetAddressOf() );
		CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\sand.dds", nullptr, textureSand.GetAddressOf() );
		CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\sky.dds", nullptr, textureSky.GetAddressOf() );
	
		// setup camera
		camera = std::make_shared<Camera>(
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
		cubeGeometry.vertexBuffer = vb_cube.Get();
		cubeGeometry.indexBuffer = ib_cube.Get();
		cubeGeometry.numberOfIndices = 36;
		cubeGeometry.vertexBufferOffset = 0;
		cubeGeometry.vertexBufferStride = sizeof( SimpleVertex );

		Geometry planeGeometry;
		planeGeometry.vertexBuffer = vb_plane.Get();
		planeGeometry.indexBuffer = ib_plane.Get();
		planeGeometry.numberOfIndices = 6;
		planeGeometry.vertexBufferOffset = 0;
		planeGeometry.vertexBufferStride = sizeof( SimpleVertex );

		// initialize floor
		std::unique_ptr<GameObject> gameObject = std::make_unique<GameObject>( "Floor" );
		gameObject->GetTransform()->SetInitialPosition( 0.0f, 0.0f, 0.0f );
		gameObject->GetTransform()->SetScale( 50.0f, 50.0f, 50.0f );
		gameObject->GetTransform()->SetRotation( XMConvertToRadians( 90.0f ), 0.0f, 0.0f );
		gameObject->GetAppearance()->SetTextureRV( textureSand.Get() );
		gameObject->GetAppearance()->SetGeometryData( planeGeometry );
		gameObject->GetAppearance()->SetMaterial( noSpecMaterial );
		gameObjects.push_back( std::move( gameObject ) );

		// initialize cubes
		for ( auto i = 0; i < NUMBER_OF_CUBES; i++ )
		{
			gameObject = std::make_unique<GameObject>( "Cube " + i );
			gameObject->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
			gameObject->GetTransform()->SetInitialPosition( -4.0f + ( i * 2.0f ), 0.5f, 10.0f );
			gameObject->GetAppearance()->SetTextureRV( textureStone.Get() );
			gameObject->GetAppearance()->SetGeometryData( cubeGeometry );
			gameObject->GetAppearance()->SetMaterial( shinyMaterial );
			gameObjects.push_back( std::move( gameObject ) );
		}

		// initialize donut
		gameObject = std::make_unique<GameObject>( "Donut" );
		gameObject->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
		gameObject->GetTransform()->SetInitialPosition( -4.0f, 0.5f, 10.0f );
		gameObject->GetAppearance()->SetTextureRV( textureHercules.Get() );
		gameObject->GetAppearance()->SetGeometryData( herculesGeometry );
		gameObject->GetAppearance()->SetMaterial( shinyMaterial );
		gameObjects.push_back( std::move( gameObject ) );

		// initialize skybox
		skybox = std::make_unique<GameObject>( "Skybox" );
		skybox->GetTransform()->SetScale( 200.0f, 200.0f, 200.0f );
		skybox->GetTransform()->SetInitialPosition( camera->GetPosition() );
		skybox->GetAppearance()->SetTextureRV( textureSky.Get() );
		skybox->GetAppearance()->SetGeometryData( cubeGeometry );
		skybox->GetAppearance()->SetMaterial( noSpecMaterial );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

	return true;
}

void Graphics::Update( float dt )
{
	skybox->Update();
	camera->Update();
	for ( int i = 0; i < gameObjects.size(); i++ )
		gameObjects[i]->Update();
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
	cb_vs_matrix.data.IsSkybox = 0.0f;

	// Render Scene Objects
	for ( int i = 0; i < gameObjects.size(); i++ )
	{
		// Get Materials
		Material material = gameObjects[i]->GetAppearance()->GetMaterial();
		cb_vs_matrix.data.surface.AmbientMtrl = material.ambient;
		cb_vs_matrix.data.surface.DiffuseMtrl = material.diffuse;
		cb_vs_matrix.data.surface.SpecularMtrl = material.specular;
		cb_vs_matrix.data.World = XMMatrixTranspose( gameObjects[i]->GetTransform()->GetWorldMatrix() );

		// Set Textures
		if ( gameObjects[i]->GetAppearance()->HasTexture() )
		{
			ID3D11ShaderResourceView* textureRV = gameObjects[i]->GetAppearance()->GetTextureRV();
			context->PSSetShaderResources( 0, 1, &textureRV );
			cb_vs_matrix.data.HasTexture = 1.0f;
		}
		else
		{
			cb_vs_matrix.data.HasTexture = 0.0f;
		}

		if ( !cb_vs_matrix.ApplyChanges() ) return;
		gameObjects[i]->Draw( context.Get() );
	}

	// Render Cubemap
	rasterizerStates["Cubemap"]->Bind( *this );
	skybox->GetTransform()->SetPosition( camera->GetPosition() );
	cb_vs_matrix.data.World = XMMatrixTranspose( skybox->GetTransform()->GetWorldMatrix() );
	cb_vs_matrix.data.IsSkybox = 1.0f;
	ID3D11ShaderResourceView* skyboxTexture = skybox->GetAppearance()->GetTextureRV();
	context->PSSetShaderResources( 0, 1, &skyboxTexture );
	if ( !cb_vs_matrix.ApplyChanges() ) return;
	skybox->Draw( context.Get() );

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