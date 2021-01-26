#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include "Camera.h"
#include "OBJLoader.h"
#include "GameObject.h"
#include "../resource.h"
#include "DDSTextureLoader.h"

#define NUMBER_OF_CUBES 5
#define FPS_60 1.0f/60.0f

struct Light
{
	v4df AmbientLight;
	v4df DiffuseLight;
	v4df SpecularLight;

	float SpecularPower;
	v3df LightVecW;
};

struct SurfaceInfo
{
	v4df AmbientMtrl;
	v4df DiffuseMtrl;
	v4df SpecularMtrl;
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
public:
	Application();
	bool Initialise( HINSTANCE hInstance, int nCmdShow );
	bool HandleKeyboard( MSG msg );
	void Update();
	void Draw();

private:
	bool InitDevice();
	bool InitWindow( HINSTANCE hInstance, int nCmdShow );
	bool CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );
	bool InitShadersAndInputLayout();
	bool InitVertexBuffer();
	bool InitIndexBuffer();

	// setup
	HWND _hWnd = nullptr;
	HINSTANCE _hInst = nullptr;
	D3D_DRIVER_TYPE _driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL _featureLevel = D3D_FEATURE_LEVEL_11_0;

	// pipeline
	Microsoft::WRL::ComPtr<ID3D11Device> _pd3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> _pImmediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> _pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _pRenderTargetView;
	
	// shaders
	Microsoft::WRL::ComPtr<ID3D11VertexShader> _pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> _pVertexLayout;

	// buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pPlaneVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pPlaneIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _pConstantBuffer;

	// depth stencil
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSLessEqual;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> _depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> _pSamplerLinear;

	// textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pTextureRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pGroundTextureRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _pHerculesTextureRV;

	// rasterizers
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> RSCullNone;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> CWcullMode;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> CCWcullMode;

	// objects
	Light basicLight;
	int objectToUse = 1;
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
	UINT _renderHeight = 1080;
	UINT _renderWidth = 1920;
};

#endif