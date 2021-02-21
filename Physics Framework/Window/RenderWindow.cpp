#include "stdafx.h"
#include "resource.h"
#include "WindowContainer.h"

bool RenderWindow::Initialize( WindowContainer* pWindowContainer, HINSTANCE hInstance, const std::string& windowName, const std::string& windowClass, int width, int height )
{
	// register window class
	this->hInstance = hInstance;
	this->width = width;
	this->height = height;
	windowTitle = windowName;
	windowTitle_Wide = StringConverter::StringToWide( windowName );
	this->windowClass = windowClass;
	windowClass_Wide = StringConverter::StringToWide( windowClass );

	RegisterWindowClass();

	int centerScreenX = GetSystemMetrics( SM_CXSCREEN ) / 2 - width / 2;
	int centerScreenY = GetSystemMetrics( SM_CYSCREEN ) / 2 - height / 2;

	RECT windowRect;
	windowRect.left = centerScreenX;
	windowRect.top = centerScreenY;
	windowRect.right = windowRect.left + width;
	windowRect.bottom = windowRect.top + height;
	AdjustWindowRect( &windowRect, WS_OVERLAPPEDWINDOW, FALSE );

	// create window
	hWnd = CreateWindow(
		windowClass_Wide.c_str(),
		windowTitle_Wide.c_str(),
		WS_OVERLAPPEDWINDOW,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		hInstance,
		pWindowContainer
	);

	if ( hWnd == NULL )
	{
		ErrorLogger::Log( GetLastError(), "ERROR::CreateWindow Failed for window: " + windowTitle );
		return false;
	}

	// show window
	ShowWindow( hWnd, SW_SHOW );
	SetForegroundWindow( hWnd );
	SetFocus( hWnd );

	return true;
}

LRESULT CALLBACK HandleMsgRedirect( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) noexcept
{
	switch ( uMsg )
	{
	case WM_CLOSE:
		DestroyWindow( hWnd );
		return 0;

	// handle all other messages
	default:
	{
		// get ptr to window class
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
		// forward messages to window class
		return pWindow->WindowProc( hWnd, uMsg, wParam, lParam );
	}
	}
}

LRESULT CALLBACK HandleMsgSetup( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch ( uMsg )
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>( lParam );
		WindowContainer* pWindow = reinterpret_cast<WindowContainer*>( pCreate->lpCreateParams );
		if ( pWindow == nullptr )
		{
			ErrorLogger::Log( "ERROR::Pointer to window container is null during WM_NCCREATE!" );
			exit( -1 );
		}
		SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( pWindow ) );
		SetWindowLongPtr( hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( HandleMsgRedirect ) );
		return pWindow->WindowProc( hWnd, uMsg, wParam, lParam );
	}

	default:
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
}

bool RenderWindow::ProcessMessages() noexcept
{
	MSG msg;
	ZeroMemory( &msg, sizeof( MSG ) );
	while ( PeekMessage( &msg, hWnd, 0u, 0u, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	// check if the window was closed
	if ( msg.message == WM_NULL )
	{
		if ( !IsWindow( hWnd ) )
		{
			hWnd = NULL;
			UnregisterClass( windowClass_Wide.c_str(), hInstance );
			return false;
		}
	}

	return true;
}

HWND RenderWindow::GetHWND() const noexcept
{
	return hWnd;
}

void RenderWindow::RegisterWindowClass() noexcept
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof( wc );
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( hInstance, (LPCTSTR)IDI_TUTORIAL1 );
    wc.hCursor = LoadCursor( hInstance, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
	wc.lpszMenuName = NULL;
	wc.lpszClassName = windowClass_Wide.c_str();
	wc.hIconSm = LoadIcon( wc.hInstance, (LPCTSTR)IDI_TUTORIAL1 );
	RegisterClassEx( &wc );
}

RenderWindow::~RenderWindow() noexcept
{
	if ( hWnd != NULL )
	{
		UnregisterClass( windowClass_Wide.c_str(), hInstance );
		DestroyWindow( hWnd );
	}
}