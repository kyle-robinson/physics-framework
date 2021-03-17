#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include "Timer.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "WindowContainer.h"

static enum class CurrentLevel
{
	LEVEL_1,
	LEVEL_2,
	LEVEL_3
};

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
	Timer timer;
	Level1 level1;
	Level2 level2;
	Level3 level3;
	CurrentLevel currentLevel;
};

#endif