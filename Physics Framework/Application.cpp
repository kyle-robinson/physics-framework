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

	level1.Initialize( gfx );

	keyboard.DisableAutoRepeatKeys();
	keyboard.DisableAutoRepeatChars();

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
	dt *= 0.05f;
    
	while ( !mouse.EventBufferIsEmpty() )
	{
		Mouse::MouseEvent me = mouse.ReadEvent();
		if ( mouse.IsRightDown() )
		{
			if ( me.GetType() == Mouse::MouseEvent::EventType::RawMove )
			{
				level1.GetCamera()->AdjustRotation(
					static_cast<float>( me.GetPosY() ) * 0.005f,
					static_cast<float>( me.GetPosX() ) * 0.005f,
					0.0f
				);
			}
		}
	}

	// camera movement
	level1.GetCamera()->SetCameraSpeed( 0.05f );
	if ( keyboard.KeyIsPressed( VK_SHIFT ) ) level1.GetCamera()->UpdateCameraSpeed( 0.05f );
	if ( keyboard.KeyIsPressed( 'W' ) ) Camera::MoveForward( level1.GetCamera(), dt );
	if ( keyboard.KeyIsPressed( 'A' ) ) Camera::MoveLeft( level1.GetCamera(), dt );
	if ( keyboard.KeyIsPressed( 'S' ) ) Camera::MoveBackward( level1.GetCamera(), dt );
	if ( keyboard.KeyIsPressed( 'D' ) ) Camera::MoveRight( level1.GetCamera(), dt );
	if ( keyboard.KeyIsPressed( VK_SPACE ) ) Camera::MoveUp( level1.GetCamera(), dt );
	if ( keyboard.KeyIsPressed( VK_CONTROL ) ) Camera::MoveDown( level1.GetCamera(), dt );

	// x world collisions
	if ( level1.GetCamera()->GetPositionFloat3().x <= -15.0f )
		level1.GetCamera()->SetPosition( -15.0f, level1.GetCamera()->GetPositionFloat3().y, level1.GetCamera()->GetPositionFloat3().z );
	if ( level1.GetCamera()->GetPositionFloat3().x >= 15.0f )
		level1.GetCamera()->SetPosition( 15.0f, level1.GetCamera()->GetPositionFloat3().y, level1.GetCamera()->GetPositionFloat3().z );

	// y world collisions
	if ( level1.GetCamera()->GetPositionFloat3().y <= 1.0f )
		level1.GetCamera()->SetPosition( level1.GetCamera()->GetPositionFloat3().x, 1.0f, level1.GetCamera()->GetPositionFloat3().z );
	if ( level1.GetCamera()->GetPositionFloat3().y >= 10.0f)
		level1.GetCamera()->SetPosition( level1.GetCamera()->GetPositionFloat3().x, 10.0f, level1.GetCamera()->GetPositionFloat3().z );

	// z world collisions
	if ( level1.GetCamera()->GetPositionFloat3().z <= -15.0f )
		level1.GetCamera()->SetPosition( level1.GetCamera()->GetPositionFloat3().x, level1.GetCamera()->GetPositionFloat3().y, -15.0f );
	if ( level1.GetCamera()->GetPositionFloat3().z >= 15.0f )
		level1.GetCamera()->SetPosition( level1.GetCamera()->GetPositionFloat3().x, level1.GetCamera()->GetPositionFloat3().y, 15.0f );

	// particle movement
	/*for ( uint32_t i = 0; i < gfx.particles.size(); i++ )
	{
		float randomNum = fmod( static_cast<float>( rand() ), gfx.xDist + 1.0f ) - ( gfx.xDist / 2.0f );
		if ( gfx.particles[i]->GetStartTimer() < 0 )
			gfx.particles[i]->GetParticleModel()->Move( fmod( randomNum, i ), 1.5f, 0.0f );
	}*/

	level1.Update( dt );
	level1.UpdateInput( keyboard );
}

void Application::Render()
{
	level1.Render( gfx );
}