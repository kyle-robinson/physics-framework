#include "stdafx.h"
#include "Application.h"

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
    UNREFERENCED_PARAMETER( nCmdShow );

	Application theApp;
    if ( theApp.Initialize( hInstance, "DirectX 11 Physics Framwork", "TutorialWindowClass", 1280, 720 ) )
    {
        while ( theApp.ProcessMessages() == true )
        {
            theApp.Update();
            theApp.Render();
        }
    }
	//if ( !( theApp.Initialise( hInstance, nCmdShow ) ) )
	//	return -1;

    // Main message loop
    /*MSG msg = { 0 };
    while ( WM_QUIT != msg.message )
    {
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
			bool handled = false;

			if ( msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST )
				handled = theApp.HandleKeyboard( msg );
			else if ( WM_QUIT == msg.message )
				break;

			if ( !handled )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
        }
        else
        {
			theApp.Update();
            theApp.Draw();
        }
    }*/

    return 0;
}