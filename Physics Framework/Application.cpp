#include "stdafx.h"
#include "Application.h"
#include "Camera.h"

bool Application::Initialize(
	HINSTANCE hInstance,
	const std::string& windowTitle,
	const std::string& windowClass,
	int width,
	int height )
{
	if ( !renderWindow.Initialize( this, hInstance, windowTitle, windowClass, width, height ) )
		return false;

	if ( !gfx.Initialize( renderWindow.GetHWND(), width, height ) )
		return false;

	return true;
}

bool Application::ProcessMessages() noexcept
{
	return renderWindow.ProcessMessages();
}

void Application::Update()
{
	// update delta time
    static float dt = 0.0f;
    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount64();
    if ( dwTimeStart == 0 ) dwTimeStart = dwTimeCur;
	dt += ( dwTimeCur - dwTimeStart ) / 1000.0f;
	if ( dt < FPS_60 ) return;

    // read input
    while ( !keyboard.CharBufferIsEmpty() )
	{
		unsigned char ch = keyboard.ReadChar();
	}
	while ( !keyboard.KeyBufferIsEmpty() )
	{
		Keyboard::KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();
	}
	while ( !mouse.EventBufferIsEmpty() )
	{
		Mouse::MouseEvent me = mouse.ReadEvent();
		std::string outMsg = "X: ";
		outMsg += std::to_string( me.GetPosX() );
		outMsg += ", Y: ";
		outMsg += std::to_string( me.GetPosY() );
		outMsg += "\n";
		OutputDebugStringA( outMsg.c_str() );
	}

	// camera movement
	if ( keyboard.KeyIsPressed( VK_UP ) ) Camera::MoveFoward( gfx.camera );
	if ( keyboard.KeyIsPressed( VK_DOWN ) ) Camera::MoveBackward( gfx.camera );
	if ( keyboard.KeyIsPressed( VK_RIGHT ) ) Camera::MoveRight( gfx.camera );
	if ( keyboard.KeyIsPressed( VK_LEFT ) ) Camera::MoveLeft( gfx.camera );

	// object movement
	if ( GetAsyncKeyState( 'W' ) ) gfx.gameObjects[gfx.objectToUse]->GetParticleModel()->Move(  0.0f, 0.0f,  0.1f );
	if ( GetAsyncKeyState( 'A' ) ) gfx.gameObjects[gfx.objectToUse]->GetParticleModel()->Move( -0.1f, 0.0f,  0.0f );
	if ( GetAsyncKeyState( 'S' ) ) gfx.gameObjects[gfx.objectToUse]->GetParticleModel()->Move(  0.0f, 0.0f, -0.1f );
	if ( GetAsyncKeyState( 'D' ) ) gfx.gameObjects[gfx.objectToUse]->GetParticleModel()->Move(  0.1f, 0.0f,  0.0f );
	if ( GetAsyncKeyState( VK_SPACE ) ) gfx.gameObjects[gfx.objectToUse]->GetParticleModel()->Move( 0.0f, 2.0f, 0.0f );

	// reset object position
	if ( GetAsyncKeyState( 'R' ) )
		for ( int i = 0; i < gfx.gameObjects.size(); i++ )
			gfx.gameObjects[i]->GetTransform()->ResetPosition();

	// reset object forces
	if ( GetAsyncKeyState( 'F' ) )
		for ( int i = 0; i < gfx.gameObjects.size(); i++ )
			gfx.gameObjects[i]->GetParticleModel()->ResetForces();

	gfx.Update( dt );
	dt -= FPS_60;
}

void Application::Render()
{
	gfx.Draw();
}