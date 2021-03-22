#include "stdafx.h"
#include "Graphics.h"
#include "Bindables/SwapChain.h"
#include "Bindables/DepthStencil.h"
#include "Bindables/RenderTarget.h"
#include "Bindables/Rasterizer.h"
#include "Bindables/Viewport.h"
#include "Bindables/Sampler.h"

bool Graphics::Initialize( HWND hWnd, int width, int height )
{
	windowWidth = width;
	windowHeight = height;

	InitializeDirectX( hWnd );
	if ( !InitializeShaders() ) return false;
	imgui.Initialize( hWnd, device.Get(), context.Get() );
	
	return true;
}

void Graphics::InitializeDirectX( HWND hWnd )
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
	
	context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}

bool Graphics::InitializeShaders()
{
	try
	{	
		// initialize shaders
		D3D11_INPUT_ELEMENT_DESC layout_nrm[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		HRESULT hr = vertexShader.Initialize( device, L"Resources\\Shaders\\DX11 Framework.fx", layout_nrm, ARRAYSIZE( layout_nrm ) );
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

void Graphics::BeginFrame()
{
	// clear render target/depth stencil
	static float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	renderTarget->BindAsBuffer( *this, depthStencil.get(), clearColor );
	depthStencil->ClearDepthStencil( *this );

	// set render state
	rasterizerStates["Solid"]->Bind( *this );
	samplerStates["Anisotropic"]->Bind( *this );
	imgui.BeginRender();

	// bind shaders
	Shaders::BindShaders( context.Get(), vertexShader, pixelShader );
}

void Graphics::EndFrame()
{
	// imgui windows
	imgui.SpawnInstructionWindow();
	imgui.EndRender();
	
	// display frame
	HRESULT hr = swapChain->GetSwapChain()->Present( 1u, NULL );
	if ( FAILED( hr ) )
	{
		hr == DXGI_ERROR_DEVICE_REMOVED ?
			ErrorLogger::Log( device->GetDeviceRemovedReason(), "Swap Chain. Graphics device removed!" ) :
			ErrorLogger::Log( hr, "Swap Chain failed to render frame!" );
		exit( -1 );
	}
}