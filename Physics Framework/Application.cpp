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
	// Update our time
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
        if ( mouse.IsRightDown() )
		{
			if ( me.GetType() == Mouse::MouseEvent::EventType::RawMove )
			{
				
			}
		}
		if( me.GetType() == Mouse::MouseEvent::EventType::LPress )
		{
			
		}
		if( me.GetType() == Mouse::MouseEvent::EventType::Move )
		{
			
		}
	}

	if ( keyboard.KeyIsPressed( VK_UP ) )
		Camera::MoveFoward( gfx.camera );

	if ( keyboard.KeyIsPressed( VK_DOWN ) )
		Camera::MoveBackward( gfx.camera );

	if ( keyboard.KeyIsPressed( VK_RIGHT ) )
		Camera::MoveRight( gfx.camera );

	if ( keyboard.KeyIsPressed( VK_LEFT ) )
		Camera::MoveLeft( gfx.camera );

	// camera movement
	/*if ( keyboard.KeyIsPressed( 'W' ) )
		CameraMove::MoveForward( gfx.camera, dt );

	if ( keyboard.KeyIsPressed( 'A' ) )
		CameraMove::MoveLeft( gfx.cameras[gfx.cameraToUse], dt );

	if ( keyboard.KeyIsPressed( 'S' ) )
		CameraMove::MoveBackward( gfx.cameras[gfx.cameraToUse], dt );

	if ( keyboard.KeyIsPressed( 'D' ) )
		CameraMove::MoveRight( gfx.cameras[gfx.cameraToUse], dt );

	if ( keyboard.KeyIsPressed( VK_SPACE ) )
		CameraMove::MoveUp( gfx.cameras[gfx.cameraToUse], dt );

	if ( keyboard.KeyIsPressed( 'E' ) )
		CameraMove::MoveDown( gfx.cameras[gfx.cameraToUse], dt );

	// static camera
	if ( !gfx.flyCamera && gfx.cameraToUse == "Main" )
		gfx.cameras[gfx.cameraToUse]->SetPosition( gfx.cameras[gfx.cameraToUse]->GetPositionFloat3().x,
			9.0f, gfx.cameras[gfx.cameraToUse]->GetPositionFloat3().z );

	// camera world collisions
	for ( auto const& cam : gfx.cameras )
	{
		// y world collisions
		if ( cam.second->GetPositionFloat3().y <= 6.0f )
			cam.second->SetPosition( cam.second->GetPositionFloat3().x, 6.0f, cam.second->GetPositionFloat3().z );
		if ( cam.second->GetPositionFloat3().y >= 30.0f )
			cam.second->SetPosition( cam.second->GetPositionFloat3().x, 30.0f, cam.second->GetPositionFloat3().z );

		// x world colliisons
		if ( cam.second->GetPositionFloat3().x <= -125.0f )
			cam.second->SetPosition( -125.0f, cam.second->GetPositionFloat3().y, cam.second->GetPositionFloat3().z );
		if ( cam.second->GetPositionFloat3().x >= 50.0f )
			cam.second->SetPosition( 50.0f, cam.second->GetPositionFloat3().y, cam.second->GetPositionFloat3().z );

		// z world collisions
		if ( cam.second->GetPositionFloat3().z <= -100.0f )
			cam.second->SetPosition( cam.second->GetPositionFloat3().x, cam.second->GetPositionFloat3().y, -100.0f );
		if ( cam.second->GetPositionFloat3().z >= 50.0f )
			cam.second->SetPosition( cam.second->GetPositionFloat3().x, cam.second->GetPositionFloat3().y, 50.0f );
	}*/

	gfx.Update( dt );

	dt -= FPS_60;
}

void Application::Render()
{
	gfx.Draw();
}