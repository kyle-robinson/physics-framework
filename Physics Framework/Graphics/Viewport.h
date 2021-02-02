#pragma once
#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "GraphicsResource.h"
class Application;

namespace Bind
{
	class Viewport : public GraphicsResource
	{
	public:
		Viewport( Application& gfx ) : Viewport( gfx, gfx.GetWidth(), gfx.GetHeight() ) {}
		Viewport( Application& gfx, UINT width, UINT height )
		{
			viewportDesc = CD3D11_VIEWPORT( 0.0f, 0.0f, width, height );
			GetContext( gfx )->RSSetViewports( 1u, &viewportDesc );
		}
	private:
		CD3D11_VIEWPORT viewportDesc = {};
	};
}

#endif