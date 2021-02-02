#include "stdafx.h"
#include "Application.h"
#include "SwapChain.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Rasterizer.h"
#include "Viewport.h"
#include "Sampler.h"
#include "Vertices.h"
#include "Indices.h"

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch ( message )
	{
	case WM_PAINT:
		hdc = BeginPaint( hWnd, &ps );
		EndPaint( hWnd, &ps );
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}

bool Application::HandleKeyboard( MSG msg )
{
	v3df cameraPosition = _camera->GetPosition();

	switch ( msg.wParam )
	{
	// window management
	case VK_ESCAPE:
		PostQuitMessage( 0 );
		return true;

	// camera movement
	case VK_UP:
		_cameraOrbitRadius = max( _cameraOrbitRadiusMin, _cameraOrbitRadius - ( _cameraSpeed * 0.2f ) );
		return true;
	case VK_DOWN:
		_cameraOrbitRadius = min( _cameraOrbitRadiusMax, _cameraOrbitRadius + ( _cameraSpeed * 0.2f ) );
		return true;
	case VK_RIGHT:
		_cameraOrbitAngleXZ -= _cameraSpeed;
		return true;
	case VK_LEFT:
		_cameraOrbitAngleXZ += _cameraSpeed;
		return true;

	// set object to use
	case '1': objectToUse = 1; return true;
	case '2': objectToUse = 2; return true;
	case '3': objectToUse = 3; return true;
	case '4': objectToUse = 4; return true;
	case '5': objectToUse = 5; return true;
	}

	return false;
}

bool Application::Initialise( HINSTANCE hInstance, int nCmdShow )
{
	RECT rc;
	GetClientRect( hWnd, &rc );
	_WindowWidth = rc.right - rc.left;
	_WindowHeight = rc.bottom - rc.top;

	if ( !InitializeWindow( hInstance, nCmdShow ) ) return false;
	if ( !InitializeDirectX() ) return false;
	if ( !InitializeShaders() ) return false;
	if ( !InitializeObjects() ) return false;

	CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\stone.dds", nullptr, textureStone.GetAddressOf() );
	CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\floor.dds", nullptr, textureGround.GetAddressOf() );
	CreateDDSTextureFromFile( device.Get(), L"Resources\\Textures\\Hercules_COLOR.dds", nullptr, textureHercules.GetAddressOf() );
	
    // setup Camera
	v3df eye( 0.0f, 2.0f, -1.0f );
	v3df at( 0.0f, 2.0f, 0.0f );
	v3df up( 0.0f, 1.0f, 0.0f );

	_camera = std::make_shared<Camera>( eye, at, up, static_cast<float>( _renderWidth ), static_cast<float>( _renderHeight ), 0.01f, 200.0f );

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
	gameObject->GetTransform()->SetScale( 15.0f, 15.0f, 15.0f );
	gameObject->GetTransform()->SetRotation( XMConvertToRadians( 90.0f ), 0.0f, 0.0f );
	gameObject->GetAppearance()->SetTextureRV( textureGround.Get() );
	gameObject->GetAppearance()->SetGeometryData( planeGeometry );
	gameObject->GetAppearance()->SetMaterial( noSpecMaterial );
	_gameObjects.push_back( std::move( gameObject ) );

	// initialize cubes
	for ( auto i = 0; i < NUMBER_OF_CUBES; i++ )
	{
		gameObject = std::make_unique<GameObject>( "Cube " + i );
		gameObject->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
		gameObject->GetTransform()->SetInitialPosition( -4.0f + ( i * 2.0f ), 0.5f, 10.0f );
		gameObject->GetAppearance()->SetTextureRV( textureStone.Get() );
		gameObject->GetAppearance()->SetGeometryData( cubeGeometry );
		gameObject->GetAppearance()->SetMaterial( shinyMaterial );
		_gameObjects.push_back( std::move( gameObject ) );
	}

	// initialize donut
	gameObject = std::make_unique<GameObject>( "Donut" );
	gameObject->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
	gameObject->GetTransform()->SetInitialPosition( -4.0f, 0.5f, 10.0f );
	gameObject->GetAppearance()->SetTextureRV( textureHercules.Get() );
	gameObject->GetAppearance()->SetGeometryData( herculesGeometry );
	gameObject->GetAppearance()->SetMaterial( shinyMaterial );
	_gameObjects.push_back( std::move( gameObject ) );

	return true;
}

bool Application::InitializeShaders()
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

bool Application::InitializeObjects()
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
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

	return true;
}

bool Application::InitializeWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon( hInstance, (LPCTSTR)IDI_TUTORIAL1 );
	wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon( wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1 );
	
	if ( !RegisterClassEx( &wcex ) ) return false;

	// Create window
	this->hInstance = hInstance;
	RECT rc = { 0, 0, 960, 540 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	hWnd = CreateWindow( L"TutorialWindowClass", L"FGGC Semester 2 Framework", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr );
		
	if ( !hWnd ) return false;
	ShowWindow( hWnd, nCmdShow );

    return true;
}

bool Application::InitializeDirectX()
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
		rasterizerStates["Solid"]->Bind( *this );

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

void Application::Update()
{
    // Update our time
    static float dt = 0.0f;
    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount64();
    if ( dwTimeStart == 0 ) dwTimeStart = dwTimeCur;
	dt += ( dwTimeCur - dwTimeStart ) / 1000.0f;
	if ( dt < FPS_60 ) return;

	// Set Object Velocities
	if ( GetAsyncKeyState( 'W' ) ) _gameObjects[objectToUse]->GetParticleModel()->Move(  0.0f, 0.0f,  0.1f );
	if ( GetAsyncKeyState( 'A' ) ) _gameObjects[objectToUse]->GetParticleModel()->Move( -0.1f, 0.0f,  0.0f );
	if ( GetAsyncKeyState( 'S' ) ) _gameObjects[objectToUse]->GetParticleModel()->Move(  0.0f, 0.0f, -0.1f );
	if ( GetAsyncKeyState( 'D' ) ) _gameObjects[objectToUse]->GetParticleModel()->Move(  0.1f, 0.0f,  0.0f );
	if ( GetAsyncKeyState( VK_SPACE ) ) _gameObjects[objectToUse]->GetParticleModel()->Move( 0.0f, 2.0f, 0.0f );

	// Reset Object Positions & Forces
	if ( GetAsyncKeyState( 'R' ) )
		for ( int i = 0; i < _gameObjects.size(); i++ )
			_gameObjects[i]->GetTransform()->ResetPosition();
	if ( GetAsyncKeyState( 'F' ) )
		for ( int i = 0; i < _gameObjects.size(); i++ )
			_gameObjects[i]->GetParticleModel()->ResetForces();

	// Update Camera
	float angleAroundZ = XMConvertToRadians( _cameraOrbitAngleXZ );
	float x = _cameraOrbitRadius * cos( angleAroundZ );
	float z = _cameraOrbitRadius * sin( angleAroundZ );

	v3df cameraPos = _camera->GetPosition();
	cameraPos = { x, cameraPos[1], z };
	_camera->SetPosition( cameraPos );
	_camera->Update();

	// Update Objects
	for ( int i = 0; i < _gameObjects.size(); i++ )
		_gameObjects[i]->Update();

	dt -= FPS_60;
}

void Application::Draw()
{
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	renderTarget->BindAsBuffer( *this, depthStencil.get(), clearColor );
    depthStencil->ClearDepthStencil( *this );

    // Setup Buffers
	Shaders::BindShaders( context.Get(), vertexShader, pixelShader );
	context->VSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );
	context->PSSetConstantBuffers( 0, 1, cb_vs_matrix.GetAddressOf() );

	// Setup Constant Buffer
	cb_vs_matrix.data.View = XMMatrixTranspose( XMLoadFloat4x4( &_camera->GetView() ) );
	cb_vs_matrix.data.Projection = XMMatrixTranspose( XMLoadFloat4x4( &_camera->GetProjection() ) );
	cb_vs_matrix.data.light = basicLight;
	cb_vs_matrix.data.EyePosW = _camera->GetPosition();

	// Render Scene Objects
	for ( int i = 0; i < _gameObjects.size(); i++ )
	{
		// Get Materials
		Material material = _gameObjects[i]->GetAppearance()->GetMaterial();
		cb_vs_matrix.data.surface.AmbientMtrl = material.ambient;
		cb_vs_matrix.data.surface.DiffuseMtrl = material.diffuse;
		cb_vs_matrix.data.surface.SpecularMtrl = material.specular;
		cb_vs_matrix.data.World = XMMatrixTranspose( _gameObjects[i]->GetTransform()->GetWorldMatrix() );

		// Set Textures
		if ( _gameObjects[i]->GetAppearance()->HasTexture() )
		{
			ID3D11ShaderResourceView* textureRV = _gameObjects[i]->GetAppearance()->GetTextureRV();
			context->PSSetShaderResources( 0, 1, &textureRV );
			cb_vs_matrix.data.HasTexture = 1.0f;
		}
		else
		{
			cb_vs_matrix.data.HasTexture = 0.0f;
		}

		if ( !cb_vs_matrix.ApplyChanges() ) return;
		_gameObjects[i]->Draw( context.Get() );
	}

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