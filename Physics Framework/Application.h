#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include "Logging/Timer.h"
#include "Levels/Level1.h"
#include "Levels/Level2.h"
#include "Levels/Level3.h"
#include "Graphics/Window/WindowContainer.h"

class Application : public WindowContainer
{
public:
	bool Initialize(
		HINSTANCE hInstance,
		const std::string& windowTitle,
		const std::string& windowClass,
		int width,
		int height
	);
	bool ProcessMessages() noexcept;
	void Update();
	void Render();
private:
	void SpawnControlWindow();

	enum ActiveLevel
	{
		LEVEL_1,
		LEVEL_2,
		LEVEL_3
	} activeLevel = LEVEL_1;

	Timer timer;
	Level1 level1;
	Level2 level2;
	Level3 level3;
};

#endif