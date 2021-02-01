#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "Camera.h"
#include "Shaders.h"
#include "OBJLoader.h"
#include "GameObject.h"
#include "../resource.h"
#include "DDSTextureLoader.h"

#define NUMBER_OF_CUBES 5
#define FPS_60 1.0f/60.0f
#define MAX_PARTICLE_COUNT 10

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
	bool Initialise( HINSTANCE hInstance, int nCmdShow );
	bool HandleKeyboard( MSG msg );
	void Update();
	void Draw();

private:
	bool InitializeDirectX();
	bool InitializeWindow( HINSTANCE hInstance, int nCmdShow );
	bool InitializeShaders();
	bool InitializeObjects();

	// setup
	HWND hWnd = nullptr;
	HINSTANCE hInstance = nullptr;

	// pipeline
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	// buffers
	VertexBuffer<SimpleVertex> vb_cube;
	IndexBuffer ib_cube;
	VertexBuffer<SimpleVertex> vb_plane;
	IndexBuffer ib_plane;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	// depth stencil
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerAnisotropic;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	// textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureStone;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureGround;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureHercules;

	// rasterizers
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerCW;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerCCW;

	// shaders
	VertexShader vertexShader;
	PixelShader pixelShader;

	// objects
	Light basicLight;
	int objectToUse = 1;
	MeshData objMeshData;
	std::shared_ptr<Camera> _camera;
	std::vector<std::unique_ptr<GameObject>> _gameObjects;

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