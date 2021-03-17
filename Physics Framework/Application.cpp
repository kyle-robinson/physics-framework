#include "stdafx.h"
#include "Application.h"

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
	level2.Initialize( gfx );
	//level3.Initialize( gfx );
	currentLevel = CurrentLevel::LEVEL_1;

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

	if ( keyboard.KeyIsPressed( VK_F1 ) ) currentLevel = CurrentLevel::LEVEL_1;
	if ( keyboard.KeyIsPressed( VK_F2 ) ) currentLevel = CurrentLevel::LEVEL_2;
	if ( keyboard.KeyIsPressed( VK_F3 ) ) currentLevel = CurrentLevel::LEVEL_3;

	// update current level
	switch ( currentLevel )
	{
	case CurrentLevel::LEVEL_1: level1.Update( mouse, keyboard, dt ); break;
	case CurrentLevel::LEVEL_2: level2.Update( mouse, keyboard, dt ); break;
	case CurrentLevel::LEVEL_3: /*level3.Update( mouse, keyboard, dt );*/ break;
	default: __debugbreak(); break;
	}
}

void Application::Render()
{
	// render current level
	switch ( currentLevel )
	{
	case CurrentLevel::LEVEL_1: level1.Render( gfx ); break;
	case CurrentLevel::LEVEL_2: level2.Render( gfx ); break;
	case CurrentLevel::LEVEL_3: /*level3.Render( gfx );*/ break;
	default: __debugbreak(); break;
	}
}