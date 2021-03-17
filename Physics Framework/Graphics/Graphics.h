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

#include "../Contact.h"
#include "../CollisionData.h"
#include "../ContactResolver.h"
#include "../CollisionDetector.h"
#include "../CollisionPrimitives.h"

#define PARTICLE_COUNT 20
#define NUMBER_OF_CUBES 5
#define MAX_CONTACTS 10

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
	bool Initialize( HWND hWnd, int width, int height );
	void BeginFrame();
	void EndFrame();

	UINT GetWidth() const noexcept { return windowWidth; };
	UINT GetHeight() const noexcept { return windowHeight; };
	ImGuiManager GetImGuiManager() const noexcept { return imgui; }
	std::shared_ptr<Bind::Rasterizer> GetRasterizer( const std::string& index ) noexcept { return rasterizerStates[index]; }
private:
	void InitializeDirectX( HWND hWnd );
	bool InitializeShaders();

	// Device/Context
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	// Pipeline States
	std::shared_ptr<Bind::Viewport> viewport;
	std::shared_ptr<Bind::SwapChain> swapChain;
	std::shared_ptr<Bind::DepthStencil> depthStencil;
	std::shared_ptr<Bind::RenderTarget> renderTarget;
	std::unordered_map<std::string, std::shared_ptr<Bind::Sampler>> samplerStates;
	std::unordered_map<std::string, std::shared_ptr<Bind::Rasterizer>> rasterizerStates;

	// Shaders
	VertexShader vertexShader;
	PixelShader pixelShader;

	// Local Variables
	UINT windowWidth;
	UINT windowHeight;
	ImGuiManager imgui;
};

#endif