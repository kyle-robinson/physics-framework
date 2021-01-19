#pragma once

#include <vector>
#include <memory>
#include <windows.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

#include "Camera.h"
#include "resource.h"
#include "OBJLoader.h"
#include "Structures.h"
#include "GameObject.h"
#include "DDSTextureLoader.h"

#define NUMBER_OF_CUBES 1
#define FPS_60 1.0f/60.0f

using namespace DirectX;

struct SurfaceInfo
{
	v4df AmbientMtrl;
	v4df DiffuseMtrl;
	v4df SpecularMtrl;
};

struct Light
{
	v4df AmbientLight;
	v4df DiffuseLight;
	v4df SpecularLight;

	float SpecularPower;
	v3df LightVecW;
};

struct ConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;
	
	SurfaceInfo surface;

	Light light;

	v3df EyePosW;
	float HasTexture;
};

class Application
{
private:
	HWND _hWnd = nullptr;
	HINSTANCE _hInst = nullptr;
	D3D_DRIVER_TYPE _driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL _featureLevel = D3D_FEATURE_LEVEL_11_0;
	Microsoft::WRL::ComPtr<ID3D11Device> _pd3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _pImmediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> _pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> _pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> _pVertexLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> _pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pIndexBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> _pPlaneVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pPlaneIndexBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> _pConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> _depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pTextureRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pGroundTextureRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pHerculesTextureRV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> _pSamplerLinear;

	Light basicLight;
	MeshData objMeshData;
	std::shared_ptr<Camera> _camera;
	std::vector<std::shared_ptr<GameObject>> _gameObjects;

	float _cameraOrbitRadius = 7.0f;
	float _cameraOrbitRadiusMin = 2.0f;
	float _cameraOrbitRadiusMax = 50.0f;
	float _cameraOrbitAngleXZ = -90.0f;
	float _cameraSpeed = 2.0f;

	UINT _WindowHeight = 0;
	UINT _WindowWidth = 0;

	// Render dimensions - Change here to alter screen resolution
	UINT _renderHeight = 1080;
	UINT _renderWidth = 1920;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSLessEqual;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSCullNone;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> CCWcullMode;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> CWcullMode;

private:
	bool InitWindow( HINSTANCE hInstance, int nCmdShow );
	bool InitDevice();
	bool CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );
	bool InitShadersAndInputLayout();
	bool InitVertexBuffer();
	bool InitIndexBuffer();

public:
	Application();

	bool Initialise( HINSTANCE hInstance, int nCmdShow );
	bool HandleKeyboard( MSG msg );

	void Update();
	void Draw();
};

