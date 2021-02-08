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
	timer.Start();

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
	float dt = timer.GetMilliSecondsElapsed();
	timer.Restart();
	dt *= 0.1f;

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
		if ( mouse.IsRightDown() )
		{
			if ( me.GetType() == Mouse::MouseEvent::EventType::RawMove )
			{
				gfx.camera->AdjustRotation(
					static_cast<float>( me.GetPosY() ) * 0.005f,
					static_cast<float>( me.GetPosX() ) * 0.005f,
					0.0f
				);
			}
		}
	}

	// camera movement
	gfx.camera->SetCameraSpeed( 0.05f );
	if ( keyboard.KeyIsPressed( VK_SHIFT ) )
		gfx.camera->UpdateCameraSpeed( 0.05f );

	if ( keyboard.KeyIsPressed( 'W' ) ) Camera::MoveForward( gfx.camera, dt );
	if ( keyboard.KeyIsPressed( 'A' ) ) Camera::MoveLeft( gfx.camera, dt );
	if ( keyboard.KeyIsPressed( 'S' ) ) Camera::MoveBackward( gfx.camera, dt );
	if ( keyboard.KeyIsPressed( 'D' ) ) Camera::MoveRight( gfx.camera, dt );
	if ( keyboard.KeyIsPressed( VK_SPACE ) ) Camera::MoveUp( gfx.camera, dt );
	if ( keyboard.KeyIsPressed( VK_CONTROL ) ) Camera::MoveDown( gfx.camera, dt );

	// select cube to move
	static int cubeToUse = 0;
	if ( keyboard.KeyIsPressed( '1' ) ) cubeToUse = 0;
	if ( keyboard.KeyIsPressed( '2' ) ) cubeToUse = 1;
	if ( keyboard.KeyIsPressed( '3' ) ) cubeToUse = 2;
	if ( keyboard.KeyIsPressed( '4' ) ) cubeToUse = 3;
	if ( keyboard.KeyIsPressed( '5' ) ) cubeToUse = 4;

	// object movement
	if ( keyboard.KeyIsPressed( VK_UP ) ) gfx.cubes[cubeToUse]->GetParticleModel()->Move(  0.0f, 0.0f,  0.1f );
	if ( keyboard.KeyIsPressed( VK_LEFT ) ) gfx.cubes[cubeToUse]->GetParticleModel()->Move( -0.1f, 0.0f,  0.0f );
	if ( keyboard.KeyIsPressed( VK_DOWN ) ) gfx.cubes[cubeToUse]->GetParticleModel()->Move(  0.0f, 0.0f, -0.1f );
	if ( keyboard.KeyIsPressed( VK_RIGHT ) ) gfx.cubes[cubeToUse]->GetParticleModel()->Move(  0.1f, 0.0f,  0.0f );
	if ( keyboard.KeyIsPressed( VK_HOME ) ) gfx.cubes[cubeToUse]->GetParticleModel()->Move( 0.0f, 2.0f, 0.0f );

	// particle movement
	for ( unsigned int i = 0; i < gfx.particles.size(); i++ )
	{
		float randomNum = fmod( static_cast<float>( rand() ), gfx.xDist + 1.0f ) - ( gfx.xDist / 2.0f );
		if ( gfx.particles[i]->GetStartTimer() < 0 )
			gfx.particles[i]->GetParticleModel()->Move( fmod( randomNum, i ), 1.5f, 0.0f );
	}

	// reset object position
	if ( keyboard.KeyIsPressed( 'R' ) )
		for ( unsigned int i = 0; i < gfx.cubes.size(); i++ )
			gfx.cubes[i]->GetTransform()->ResetPosition();

	// reset object forces
	if ( keyboard.KeyIsPressed( 'F' ) )
		for ( unsigned int i = 0; i < gfx.cubes.size(); i++ )
			gfx.cubes[i]->GetParticleModel()->ResetForces();

	gfx.Update( dt );
}

void Application::Render()
{
	gfx.Draw();
}