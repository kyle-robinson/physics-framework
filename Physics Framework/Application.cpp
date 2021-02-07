#include "stdafx.h"
#include "Application.h"
#include "Camera.h"
#include <cstdlib>
#include <ctime>

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

	// select cube to move
	static int cubeToUse = 0;
	if ( keyboard.KeyIsPressed( '1' ) ) cubeToUse = 0;
	if ( keyboard.KeyIsPressed( '2' ) ) cubeToUse = 1;
	if ( keyboard.KeyIsPressed( '3' ) ) cubeToUse = 2;
	if ( keyboard.KeyIsPressed( '4' ) ) cubeToUse = 3;
	if ( keyboard.KeyIsPressed( '5' ) ) cubeToUse = 4;

	// object movement
	if ( GetAsyncKeyState( 'W' ) ) gfx.cubes[cubeToUse]->GetParticleModel()->Move(  0.0f, 0.0f,  0.1f );
	if ( GetAsyncKeyState( 'A' ) ) gfx.cubes[cubeToUse]->GetParticleModel()->Move( -0.1f, 0.0f,  0.0f );
	if ( GetAsyncKeyState( 'S' ) ) gfx.cubes[cubeToUse]->GetParticleModel()->Move(  0.0f, 0.0f, -0.1f );
	if ( GetAsyncKeyState( 'D' ) ) gfx.cubes[cubeToUse]->GetParticleModel()->Move(  0.1f, 0.0f,  0.0f );
	if ( GetAsyncKeyState( VK_SPACE ) ) gfx.cubes[cubeToUse]->GetParticleModel()->Move( 0.0f, 2.0f, 0.0f );

	// particle movement
	for ( unsigned int i = 0; i < gfx.particles.size(); i++ )
	{
		float randomNum = fmod( static_cast<float>( rand() ), 21.0f ) - 10.0f;
		if ( gfx.particles[i]->GetStartTimer() < 0 )
			gfx.particles[i]->GetParticleModel()->Move( fmod( randomNum, i ), 1.5f, 0.0f );
	}

	// reset object position
	if ( GetAsyncKeyState( 'R' ) )
		for ( unsigned int i = 0; i < gfx.cubes.size(); i++ )
			gfx.cubes[i]->GetTransform()->ResetPosition();

	// reset object forces
	if ( GetAsyncKeyState( 'F' ) )
		for ( unsigned int i = 0; i < gfx.cubes.size(); i++ )
			gfx.cubes[i]->GetParticleModel()->ResetForces();

	gfx.Update( dt );
	dt -= FPS_60;
}

void Application::Render()
{
	gfx.Draw();
}