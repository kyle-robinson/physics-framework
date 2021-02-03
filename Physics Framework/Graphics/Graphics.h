#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Mouse.h"
#include "Keyboard.h"

#include "Camera.h"
#include "Shaders.h"
#include "resource.h"
#include "OBJLoader.h"
#include "GameObject.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "DDSTextureLoader.h"

#define NUMBER_OF_CUBES 5
#define FPS_60 1.0f/60.0f

namespace Bind
{
	class DepthStencil;
	class RenderTarget;
	class Rasterizer;
	class SwapChain;
	class Viewport;
	class Sampler;
}

class Graphics
{
	friend class GraphicsResource;
public:
	UINT GetWidth() const noexcept { return windowWidth; };
	UINT GetHeight() const noexcept { return windowHeight; };
	//bool Initialise( HINSTANCE hInstance, int nCmdShow );
	bool Initialize( HWND hWnd, int width, int height );
	//bool HandleKeyboard( MSG msg );
	void Update( float dt );
	void Draw();
public:
	std::shared_ptr<Camera> camera;
private:
	bool InitializeDirectX( HWND hWnd );
	//bool InitializeWindow( HINSTANCE hInstance, int nCmdShow );
	bool InitializeShaders();
	bool InitializeScene();

	// setup
	//HWND hWnd = nullptr;
	//HINSTANCE hInstance = nullptr;

	// pipeline
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	
	std::shared_ptr<Bind::Viewport> viewport;
	std::shared_ptr<Bind::SwapChain> swapChain;
	std::shared_ptr<Bind::DepthStencil> depthStencil;
	std::shared_ptr<Bind::RenderTarget> renderTarget;
	std::map<std::string, std::shared_ptr<Bind::Sampler>> samplerStates;
	std::map<std::string, std::shared_ptr<Bind::Rasterizer>> rasterizerStates;

	// buffers
	VertexBuffer<SimpleVertex> vb_cube;
	IndexBuffer ib_cube;
	VertexBuffer<SimpleVertex> vb_plane;
	IndexBuffer ib_plane;
	ConstantBuffer<CB_VS_matrix> cb_vs_matrix;

	// textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSky;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSand;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureStone;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureGround;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureHercules;

	// shaders
	VertexShader vertexShader;
	PixelShader pixelShader;

	// input
	Mouse mouse;
	Keyboard keyboard;

	// objects
	Light basicLight;
	int objectToUse = 1;
	MeshData objMeshData;
	std::unique_ptr<GameObject> skybox;
	std::vector<std::unique_ptr<GameObject>> gameObjects;

	UINT windowWidth = 1280;
	UINT windowHeight = 720;
};

#endif