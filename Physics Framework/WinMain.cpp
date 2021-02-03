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

    return 0;
}