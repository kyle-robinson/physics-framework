#include "stdafx.h"
#include "Application.h"

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
	GetClientRect( _hWnd, &rc );
	_WindowWidth = rc.right - rc.left;
	_WindowHeight = rc.bottom - rc.top;

	if ( !InitWindow( hInstance, nCmdShow ) ) return false;
	if ( !InitDevice() ) return false;
	if ( !InitShadersAndInputLayout() ) return false;
	if ( !InitVertexBuffer() ) return false;
	if ( !InitIndexBuffer() ) return false;

	CreateDDSTextureFromFile( _pd3dDevice.Get(), L"Resources\\Textures\\stone.dds", nullptr, _pTextureRV.GetAddressOf() );
	CreateDDSTextureFromFile( _pd3dDevice.Get(), L"Resources\\Textures\\floor.dds", nullptr, _pGroundTextureRV.GetAddressOf() );
	CreateDDSTextureFromFile( _pd3dDevice.Get(), L"Resources\\Textures\\Hercules_COLOR.dds", nullptr, _pHerculesTextureRV.GetAddressOf() );
	
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

	// create geometry pieces
	Geometry herculesGeometry;
	objMeshData = OBJLoader::Load( "Resources\\Models\\donut.obj", _pd3dDevice.Get() );
	herculesGeometry.indexBuffer = objMeshData.IndexBuffer;
	herculesGeometry.numberOfIndices = objMeshData.IndexCount;
	herculesGeometry.vertexBuffer = objMeshData.VertexBuffer;
	herculesGeometry.vertexBufferOffset = objMeshData.VBOffset;
	herculesGeometry.vertexBufferStride = objMeshData.VBStride;
	
	Geometry cubeGeometry;
	cubeGeometry.indexBuffer = _pIndexBuffer.Get();
	cubeGeometry.vertexBuffer = _pVertexBuffer.Get();
	cubeGeometry.numberOfIndices = 36;
	cubeGeometry.vertexBufferOffset = 0;
	cubeGeometry.vertexBufferStride = sizeof( SimpleVertex );

	Geometry planeGeometry;
	planeGeometry.indexBuffer = _pPlaneIndexBuffer.Get();
	planeGeometry.vertexBuffer = _pPlaneVertexBuffer.Get();
	planeGeometry.numberOfIndices = 6;
	planeGeometry.vertexBufferOffset = 0;
	planeGeometry.vertexBufferStride = sizeof( SimpleVertex );

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

	// initialize floor
	std::unique_ptr<GameObject> gameObject = std::make_unique<GameObject>( "Floor" );
	gameObject->GetTransform()->SetInitialPosition( 0.0f, 0.0f, 0.0f );
	gameObject->GetTransform()->SetScale( 15.0f, 15.0f, 15.0f );
	gameObject->GetTransform()->SetRotation( XMConvertToRadians( 90.0f ), 0.0f, 0.0f );
	gameObject->GetAppearance()->SetTextureRV( _pGroundTextureRV.Get() );
	gameObject->GetAppearance()->SetGeometryData( planeGeometry );
	gameObject->GetAppearance()->SetMaterial( noSpecMaterial );
	_gameObjects.push_back( std::move( gameObject ) );

	// initialize cubes
	for ( auto i = 0; i < NUMBER_OF_CUBES; i++ )
	{
		gameObject = std::make_unique<GameObject>( "Cube " + i );
		gameObject->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
		gameObject->GetTransform()->SetInitialPosition( -4.0f + ( i * 2.0f ), 0.5f, 10.0f );
		gameObject->GetAppearance()->SetTextureRV( _pTextureRV.Get() );
		gameObject->GetAppearance()->SetGeometryData( cubeGeometry );
		gameObject->GetAppearance()->SetMaterial( shinyMaterial );
		_gameObjects.push_back( std::move( gameObject ) );
	}

	// initialize donut
	gameObject = std::make_unique<GameObject>( "Donut" );
	gameObject->GetTransform()->SetScale( 0.5f, 0.5f, 0.5f );
	gameObject->GetTransform()->SetInitialPosition( -4.0f, 0.5f, 10.0f );
	gameObject->GetAppearance()->SetTextureRV( _pHerculesTextureRV.Get() );
	gameObject->GetAppearance()->SetGeometryData( herculesGeometry );
	gameObject->GetAppearance()->SetMaterial( shinyMaterial );
	_gameObjects.push_back( std::move( gameObject ) );

	return true;
}

bool Application::InitShadersAndInputLayout()
{
	try
	{	
		// Compile the vertex shader
		ID3DBlob* pVSBlob = nullptr;
		HRESULT hr = CompileShaderFromFile( L"Resources\\Shaders\\DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob );
		COM_ERROR_IF_FAILED( hr, "Failed to compile the vertex shader! Run this executable from the directory that contains the FX file!" );

		// Create the vertex shader
		hr = _pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader );
		COM_ERROR_IF_FAILED( hr, "Failed to create the vertex shader!" );
		if ( FAILED( hr ) ) pVSBlob->Release();

		// Compile the pixel shader
		ID3DBlob* pPSBlob = nullptr;
		hr = CompileShaderFromFile( L"Resources\\Shaders\\DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob );
		COM_ERROR_IF_FAILED( hr, "Failed to compile the pixel shader! Run this executable from the directory that contains the FX file!" );

		// Create the pixel shader
		hr = _pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader );
		COM_ERROR_IF_FAILED( hr, "Failed to create the pixel shader!" );
		pPSBlob->Release();
	
		// Define the input layout
		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		UINT numElements = ARRAYSIZE( layout );

		// Create the input layout
		hr = _pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &_pVertexLayout );
		COM_ERROR_IF_FAILED( hr, "Failed to create the input layout!" );
		pVSBlob->Release();

		// Set the input layout
		_pImmediateContext->IASetInputLayout( _pVertexLayout.Get() );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

	return true;
}

bool Application::InitVertexBuffer()
{
	try
	{
		// Create vertex buffer
		SimpleVertex vertices[] =
		{
			{ { -1.0f, 1.0f, -1.0f }, { -1.0f, 1.0f, -1.0f }, { 1.0f, 0.0f } },
			{ {  1.0f, 1.0f, -1.0f }, {  1.0f, 1.0f, -1.0f }, { 0.0f, 0.0f } },
			{ {  1.0f, 1.0f,  1.0f }, {  1.0f, 1.0f,  1.0f }, { 0.0f, 1.0f } },
			{ { -1.0f, 1.0f,  1.0f }, { -1.0f, 1.0f,  1.0f }, { 1.0f, 1.0f } },

			{ { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f } },
			{ {  1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f }, { 1.0f, 0.0f } },
			{ {  1.0f, -1.0f,  1.0f }, {  1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f } },
			{ { -1.0f, -1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f } },

			{ { -1.0f, -1.0f,  1.0f }, { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f } },
			{ { -1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f } },
			{ { -1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f } },
			{ { -1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f } },

			{ { 1.0f, -1.0f,  1.0f },  { 1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f } },
			{ { 1.0f, -1.0f, -1.0f },  { 1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f } },
			{ { 1.0f,  1.0f, -1.0f },  { 1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f } },
			{ { 1.0f,  1.0f,  1.0f },  { 1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f } },

			{ {-1.0f, -1.0f, -1.0f }, { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f } },
			{ { 1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f } },
			{ { 1.0f,  1.0f, -1.0f }, {  1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f } },
			{ {-1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f } },

			{ { -1.0f, -1.0f, 1.0f }, { -1.0f, -1.0f, 1.0f }, { 1.0f, 1.0f } },
			{ {  1.0f, -1.0f, 1.0f }, {  1.0f, -1.0f, 1.0f }, { 0.0f, 1.0f } },
			{ {  1.0f,  1.0f, 1.0f }, {  1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f } },
			{ { -1.0f,  1.0f, 1.0f }, { -1.0f,  1.0f, 1.0f }, { 1.0f, 0.0f } },
		};

		D3D11_BUFFER_DESC bd = { 0 };
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( SimpleVertex ) * ARRAYSIZE( vertices );
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData = { 0 };
		InitData.pSysMem = vertices;

		HRESULT hr = _pd3dDevice->CreateBuffer( &bd, &InitData, &_pVertexBuffer );
		COM_ERROR_IF_FAILED( hr, "Failed to create the cube vertex buffer!" );

		// Create vertex buffer
		SimpleVertex planeVertices[] =
		{
			{ {-1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 5.0f } },
			{ { 1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 5.0f, 5.0f } },
			{ { 1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 5.0f, 0.0f } },
			{ {-1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
		};

		bd = { 0 };
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( SimpleVertex ) * ARRAYSIZE( planeVertices );
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		InitData = { 0 };
		InitData.pSysMem = planeVertices;

		hr = _pd3dDevice->CreateBuffer( &bd, &InitData, &_pPlaneVertexBuffer );
		COM_ERROR_IF_FAILED( hr, "Failed to create the plane vertex buffer!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

	return true;
}

bool Application::InitIndexBuffer()
{
	try
	{
		// Create index buffer
		WORD indices[] =
		{
			3, 1, 0,
			2, 1, 3,

			6, 4, 5,
			7, 4, 6,

			11, 9, 8,
			10, 9, 11,

			14, 12, 13,
			15, 12, 14,

			19, 17, 16,
			18, 17, 19,

			22, 20, 21,
			23, 20, 22
		};

		D3D11_BUFFER_DESC bd = { 0 };
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( WORD ) * ARRAYSIZE( indices );     
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData = { 0 };
		InitData.pSysMem = indices;

		HRESULT hr = _pd3dDevice->CreateBuffer( &bd, &InitData, &_pIndexBuffer );
		COM_ERROR_IF_FAILED( hr, "Failed to create cube index buffer!" );

		// Create plane index buffer
		WORD planeIndices[] =
		{
			0, 3, 1,
			3, 2, 1,
		};

		bd = { 0 };
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( WORD ) * ARRAYSIZE( planeIndices );
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		InitData = { 0 };
		InitData.pSysMem = planeIndices;
		hr = _pd3dDevice->CreateBuffer( &bd, &InitData, &_pPlaneIndexBuffer );
		COM_ERROR_IF_FAILED( hr, "Failed to create plane index buffer!" );
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
		return false;
	}

	return true;
}

bool Application::InitWindow( HINSTANCE hInstance, int nCmdShow )
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
	_hInst = hInstance;
	RECT rc = { 0, 0, 960, 540 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	_hWnd = CreateWindow( L"TutorialWindowClass", L"FGGC Semester 2 Framework", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr );
		
	if ( !_hWnd ) return false;
	ShowWindow( _hWnd, nCmdShow );

    return true;
}

bool Application::CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    HRESULT hr = D3DCompileFromFile( szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob );

    if ( FAILED( hr ) )
    {
        if ( pErrorBlob != nullptr ) OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if ( pErrorBlob ) pErrorBlob->Release();
		ErrorLogger::Log( hr, "Failed to compile shader from file!" );
        return false;
    }
    if ( pErrorBlob ) pErrorBlob->Release();

    return true;
}

bool Application::InitDevice()
{
	try
	{
		UINT createDeviceFlags = 0;

	#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};

		UINT numDriverTypes = ARRAYSIZE( driverTypes );

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};

		UINT numFeatureLevels = ARRAYSIZE( featureLevels );

		UINT sampleCount = 4;

		DXGI_SWAP_CHAIN_DESC sd = { 0 };
		sd.BufferCount = 1;
		sd.BufferDesc.Width = _renderWidth;
		sd.BufferDesc.Height = _renderHeight;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = _hWnd;
		sd.SampleDesc.Count = sampleCount;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		HRESULT hr;
		for ( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
		{
			_driverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDeviceAndSwapChain( nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext );
			if ( SUCCEEDED( hr ) )
				break;
		}
		COM_ERROR_IF_FAILED( hr, "Failed to create device and swap chain!" );

		// Create a render target view
		ID3D11Texture2D* pBackBuffer = nullptr;
		hr = _pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
		COM_ERROR_IF_FAILED( hr, "Failed to create swap chain!" );

		hr = _pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &_pRenderTargetView );
		COM_ERROR_IF_FAILED( hr, "Failed to create render target view!" );
		pBackBuffer->Release();

		// Setup the viewport
		D3D11_VIEWPORT vp = { 0 };
		vp.Width = static_cast<FLOAT>( _renderWidth );
		vp.Height = static_cast<FLOAT>( _renderHeight );
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		_pImmediateContext->RSSetViewports( 1, &vp );

		// Set primitive topology
		_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		// Create the constant buffer
		D3D11_BUFFER_DESC bd = { 0 };
		bd.CPUAccessFlags = 0;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.ByteWidth = static_cast<UINT>( sizeof( ConstantBuffer ) + ( 16 - ( sizeof( ConstantBuffer ) % 16 ) ) );
		hr = _pd3dDevice->CreateBuffer( &bd, nullptr, _pConstantBuffer.GetAddressOf() );
		COM_ERROR_IF_FAILED( hr, "Failed to create constant buffer!" );

		// Depth Stencil
		D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
		depthStencilDesc.Width = _renderWidth;
		depthStencilDesc.Height = _renderHeight;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = sampleCount;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		_pd3dDevice->CreateTexture2D( &depthStencilDesc, nullptr, &_depthStencilBuffer );
		COM_ERROR_IF_FAILED( hr, "Failed to create depth stencil texture!" );

		_pd3dDevice->CreateDepthStencilView( _depthStencilBuffer.Get(), nullptr, &_depthStencilView );
		COM_ERROR_IF_FAILED( hr, "Failed to create depth stencil view!" );

		_pImmediateContext->OMSetRenderTargets( 1, _pRenderTargetView.GetAddressOf(), _depthStencilView.Get() );

		// Rasterizer
		CD3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC( CD3D11_DEFAULT{} );
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		hr = _pd3dDevice->CreateRasterizerState( &rasterizerDesc, &RSCullNone );
		COM_ERROR_IF_FAILED( hr, "Failed to create default rasterizer state!" );

		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = true;
		hr = _pd3dDevice->CreateRasterizerState( &rasterizerDesc, &CCWcullMode );
		COM_ERROR_IF_FAILED( hr, "Failed to create counter-clockwise rasterizer state!" );

		rasterizerDesc.FrontCounterClockwise = false;
		hr = _pd3dDevice->CreateRasterizerState( &rasterizerDesc, &CWcullMode );
		COM_ERROR_IF_FAILED( hr, "Failed to create clockwise rasterizer state!" );
		_pImmediateContext->RSSetState( CWcullMode.Get() );

		// Depth Stencil State
		D3D11_DEPTH_STENCIL_DESC dssDesc = { 0 };
		dssDesc.DepthEnable = true;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = _pd3dDevice->CreateDepthStencilState( &dssDesc, &DSLessEqual );
		COM_ERROR_IF_FAILED( hr, "Failed to create depth stencil state!" );

		// Sampler
		CD3D11_SAMPLER_DESC sampDesc( CD3D11_DEFAULT{} );
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		hr = _pd3dDevice->CreateSamplerState( &sampDesc, &_pSamplerLinear );
		COM_ERROR_IF_FAILED( hr, "Failed to create sampler state!" );
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
    // Clear buffers
	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView( _pRenderTargetView.Get(), ClearColor );
	_pImmediateContext->ClearDepthStencilView( _depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );

    // Setup buffers and render scene
	_pImmediateContext->IASetInputLayout( _pVertexLayout.Get() );
	_pImmediateContext->VSSetShader( _pVertexShader.Get(), nullptr, 0 );
	_pImmediateContext->PSSetShader( _pPixelShader.Get(), nullptr, 0 );
	_pImmediateContext->VSSetConstantBuffers( 0, 1, _pConstantBuffer.GetAddressOf() );
	_pImmediateContext->PSSetConstantBuffers( 0, 1, _pConstantBuffer.GetAddressOf() );
	_pImmediateContext->PSSetSamplers( 0, 1, _pSamplerLinear.GetAddressOf() );

	ConstantBuffer cb;
	cb.View = XMMatrixTranspose( XMLoadFloat4x4( &_camera->GetView() ) );
	cb.Projection = XMMatrixTranspose( XMLoadFloat4x4( &_camera->GetProjection() ) );
	cb.light = basicLight;
	cb.EyePosW = _camera->GetPosition();

	// Render all scene objects
	for ( int i = 0; i < _gameObjects.size(); i++ )
	{
		// Get render material
		Material material = _gameObjects[i]->GetAppearance()->GetMaterial();
		cb.surface.AmbientMtrl = material.ambient;
		cb.surface.DiffuseMtrl = material.diffuse;
		cb.surface.SpecularMtrl = material.specular;
		cb.World = XMMatrixTranspose( _gameObjects[i]->GetTransform()->GetWorldMatrix() );

		// Set texture
		if ( _gameObjects[i]->GetAppearance()->HasTexture() )
		{
			ID3D11ShaderResourceView* textureRV = _gameObjects[i]->GetAppearance()->GetTextureRV();
			_pImmediateContext->PSSetShaderResources( 0, 1, &textureRV );
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}
		_pImmediateContext->UpdateSubresource( _pConstantBuffer.Get(), 0, nullptr, &cb, 0, 0 );
		_gameObjects[i]->Draw( _pImmediateContext.Get() );
	}
    _pSwapChain->Present( 0, 0 );
}