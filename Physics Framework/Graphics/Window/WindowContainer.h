#pragma once
#ifndef WINDOWCONTAINER_H
#define WINDOWCONTAINER_H

#include "RenderWindow.h"
#include "../Graphics.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"

class WindowContainer
{
public:
	WindowContainer();
	LRESULT CALLBACK WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
protected:
	RenderWindow renderWindow;
	Keyboard keyboard;
	Graphics gfx;
	Mouse mouse;
};

#endif