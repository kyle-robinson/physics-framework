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
	bool Initialize( HWND hWnd, int width, int height );
	void Update( float dt );
	void Draw();

	std::unique_ptr<Camera> camera;
	std::vector<std::unique_ptr<GameObject>> cubes;
private:
	bool InitializeDirectX( HWND hWnd );
	bool InitializeShaders();
	bool InitializeScene();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSky;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSand;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureStone;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureGround;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureHercules;
	
	std::shared_ptr<Bind::Viewport> viewport;
	std::shared_ptr<Bind::SwapChain> swapChain;
	std::shared_ptr<Bind::DepthStencil> depthStencil;
	std::shared_ptr<Bind::RenderTarget> renderTarget;
	std::map<std::string, std::shared_ptr<Bind::Sampler>> samplerStates;
	std::map<std::string, std::shared_ptr<Bind::Rasterizer>> rasterizerStates;

	VertexShader vertexShader;
	PixelShader pixelShader;

	VertexBuffer<SimpleVertex> vb_cube;
	VertexBuffer<SimpleVertex> vb_plane;
	IndexBuffer ib_cube;
	IndexBuffer ib_plane;
	ConstantBuffer<CB_VS_matrix> cb_vs_matrix;

	UINT windowWidth;
	UINT windowHeight;
	Light basicLight;
	MeshData objMeshData;
	std::unique_ptr<GameObject> torus;
	std::unique_ptr<GameObject> ground;
	std::unique_ptr<GameObject> skybox;

};

#endif