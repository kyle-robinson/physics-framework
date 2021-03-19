#pragma once
#ifndef LEVEL1_H
#define LEVEL1_H

#include "LevelManager.h"
#include "../Physics/ParticleModel.h"

#define NUMBER_OF_CUBES 5

class Level1 : public LevelManager
{
public:
	void Initialize( Graphics& gfx ) override;
	void Update( Mouse& mouse, Keyboard& keyboard, float dt ) override;
	void Render( Graphics& gfx ) override;
private:
	void SpawnControlWindow();
	void SpawnInstructionWindow();
	void UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt ) override;
	void CollisionResolution( std::unique_ptr<GameObject>& cube1, std::unique_ptr<GameObject>& cube2, float dt );

	int cubeToUse = 0;
	bool useAABB = true;
	bool useThrustMovement = true;
	std::vector<std::unique_ptr<GameObject>> cubes;
};

#endif