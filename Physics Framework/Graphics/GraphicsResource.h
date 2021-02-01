#pragma once
#ifndef GRAPHICSRESOURCE_H
#define GRAPHICSRESOURCE_H

#include "../Application.h"
class Application;

class GraphicsResource
{
protected:
	static ID3D11DeviceContext* GetContext( Application& gfx ) noexcept { return gfx.context.Get(); };
	static ID3D11Device* GetDevice( Application& gfx ) noexcept { return gfx.device.Get(); };
	virtual void Bind( Application& gfx ) noexcept {};
	virtual ~GraphicsResource() = default;
};

#endif