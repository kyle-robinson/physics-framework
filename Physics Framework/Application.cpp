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

	keyboard.DisableAutoRepeatKeys();

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

	static int cubeToUse = 0;

    // read input
    while ( !keyboard.CharBufferIsEmpty() )
	{
		unsigned char ch = keyboard.ReadChar();
	}
	while ( !keyboard.KeyBufferIsEmpty() )
	{
		Keyboard::KeyboardEvent kbe = keyboard.ReadKey();
		unsigned char keycode = kbe.GetKeyCode();

		// select cube to move
		switch ( kbe.GetKeyCode() )
		{
		case '1': cubeToUse = 0; break;
		case '2': cubeToUse = 1; break;
		case '3': cubeToUse = 2; break;
		case '4': cubeToUse = 3; break;
		case '5': cubeToUse = 4; break;
		}
		
		// object movement
		switch ( kbe.GetKeyCode() )
		{
		case VK_UP: gfx.cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.0f, 0.0f,  0.1f }, 25.0f ); break;
		case VK_LEFT: gfx.cubes[cubeToUse]->GetParticleModel()->AddThrust( { -0.1f, 0.0f,  0.0f }, 25.0f ); break;
		case VK_DOWN: gfx.cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.0f, 0.0f, -0.1f }, 25.0f ); break;
		case VK_RIGHT: gfx.cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.1f, 0.0f,  0.0f }, 25.0f ); break;
		case VK_HOME: gfx.cubes[cubeToUse]->GetParticleModel()->AddThrust( { 0.0f, 0.15f, 0.0f }, 50.0f ); break;
		}
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
	if ( keyboard.KeyIsPressed( VK_SHIFT ) ) gfx.camera->UpdateCameraSpeed( 0.05f );
	if ( keyboard.KeyIsPressed( 'W' ) ) Camera::MoveForward( gfx.camera, dt );
	if ( keyboard.KeyIsPressed( 'A' ) ) Camera::MoveLeft( gfx.camera, dt );
	if ( keyboard.KeyIsPressed( 'S' ) ) Camera::MoveBackward( gfx.camera, dt );
	if ( keyboard.KeyIsPressed( 'D' ) ) Camera::MoveRight( gfx.camera, dt );
	if ( keyboard.KeyIsPressed( VK_SPACE ) ) Camera::MoveUp( gfx.camera, dt );
	if ( keyboard.KeyIsPressed( VK_CONTROL ) ) Camera::MoveDown( gfx.camera, dt );

	// x world collisions
	if ( gfx.camera->GetPositionFloat3().x <= -15.0f )
		gfx.camera->SetPosition( -15.0f, gfx.camera->GetPositionFloat3().y, gfx.camera->GetPositionFloat3().z );
	if ( gfx.camera->GetPositionFloat3().x >= 15.0f )
		gfx.camera->SetPosition( 15.0f, gfx.camera->GetPositionFloat3().y, gfx.camera->GetPositionFloat3().z );

	// y world collisions
	if ( gfx.camera->GetPositionFloat3().y <= 1.0f )
		gfx.camera->SetPosition( gfx.camera->GetPositionFloat3().x, 1.0f, gfx.camera->GetPositionFloat3().z );
	if ( gfx.camera->GetPositionFloat3().y >= 10.0f)
		gfx.camera->SetPosition( gfx.camera->GetPositionFloat3().x, 10.0f, gfx.camera->GetPositionFloat3().z );

	// z world collisions
	if ( gfx.camera->GetPositionFloat3().z <= -15.0f )
		gfx.camera->SetPosition( gfx.camera->GetPositionFloat3().x, gfx.camera->GetPositionFloat3().y, -15.0f );
	if ( gfx.camera->GetPositionFloat3().z >= 15.0f )
		gfx.camera->SetPosition( gfx.camera->GetPositionFloat3().x, gfx.camera->GetPositionFloat3().y, 15.0f );

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