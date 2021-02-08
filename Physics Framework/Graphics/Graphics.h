#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Camera.h"
#include "Shaders.h"
#include "resource.h"
#include "Particle.h"
#include "OBJLoader.h"
#include "GameObject.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "ImGuiManager.h"
#include "ConstantBuffer.h"
#include "DDSTextureLoader.h"

#define PARTICLE_COUNT 20
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

	// Global Objects
	float xDist = 5.0f;
	std::unique_ptr<Camera> camera;
	std::vector<std::unique_ptr<GameObject>> cubes;
	std::vector<std::unique_ptr<Particle>> particles;
private:
	bool InitializeDirectX( HWND hWnd );
	bool InitializeShaders();
	bool InitializeScene();

	void SpawnControlWindow( std::vector<std::unique_ptr<GameObject>>& vec );
	void SpawnControlWindow( std::vector<std::unique_ptr<Particle>>& vec );

	// Device/Context
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	// Pipeline States
	std::shared_ptr<Bind::Viewport> viewport;
	std::shared_ptr<Bind::SwapChain> swapChain;
	std::shared_ptr<Bind::DepthStencil> depthStencil;
	std::shared_ptr<Bind::RenderTarget> renderTarget;
	std::map<std::string, std::shared_ptr<Bind::Sampler>> samplerStates;
	std::map<std::string, std::shared_ptr<Bind::Rasterizer>> rasterizerStates;

	// Textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSky;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSand;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureLava;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureStone;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureGround;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureHercules;

	// Shaders
	VertexShader vertexShader;
	PixelShader pixelShader;

	// Buffers
	VertexBuffer<SimpleVertex> vb_cube;
	VertexBuffer<SimpleVertex> vb_plane;
	IndexBuffer ib_cube;
	IndexBuffer ib_plane;
	ConstantBuffer<CB_VS_matrix> cb_vs_matrix;

	// Local Variables
	UINT windowWidth;
	UINT windowHeight;
	ImGuiManager imgui;

	// Local Objects
	Light basicLight;
	MeshData objMeshData;
	std::unique_ptr<GameObject> torus;
	std::unique_ptr<GameObject> ground;
	std::unique_ptr<GameObject> skybox;
};

#endif