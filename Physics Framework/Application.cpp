#include "stdafx.h"
#include "Application.h"
#include <imgui/imgui.h>

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

	// initialize levels
	level1.Initialize( gfx );
	level2.Initialize( gfx );
	level3.Initialize( gfx );

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

	// change current level
	if ( keyboard.KeyIsPressed( VK_F1 ) ) activeLevel = LEVEL_1;
	if ( keyboard.KeyIsPressed( VK_F2 ) ) activeLevel = LEVEL_2;
	if ( keyboard.KeyIsPressed( VK_F3 ) ) activeLevel = LEVEL_3;

	// update current level
	switch ( activeLevel )
	{
	case LEVEL_1: level1.Update( mouse, keyboard, dt ); break;
	case LEVEL_2: level2.Update( mouse, keyboard, dt ); break;
	case LEVEL_3: level3.Update( mouse, keyboard, dt ); break;
	default: __debugbreak(); break;
	}
}

void Application::Render()
{
	gfx.BeginFrame();

	// render current level
	switch ( activeLevel )
	{
	case LEVEL_1: level1.Render( gfx ); break;
	case LEVEL_2: level2.Render( gfx ); break;
	case LEVEL_3: level3.Render( gfx ); break;
	default: __debugbreak(); break;
	}

	SpawnControlWindow();
	gfx.EndFrame();
}

void Application::SpawnControlWindow()
{
	if ( ImGui::Begin( "Active Level" ) )
	{
		if ( ImGui::Button( "Level 1: Forces/Collisions", ImVec2( ImGui::GetWindowSize().x, 0.0f ) ) ) activeLevel = LEVEL_1;
		if ( ImGui::Button( "Level 2: Particle System", ImVec2( ImGui::GetWindowSize().x, 0.0f ) ) ) activeLevel = LEVEL_2;
		if ( ImGui::Button( "Level 3: Rigid Bodies", ImVec2( ImGui::GetWindowSize().x, 0.0f ) ) ) activeLevel = LEVEL_3;
	}
	ImGui::End();
}