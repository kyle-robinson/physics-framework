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
	void SpawnControlWindow( std::vector<std::unique_ptr<GameObject>>& vec );
private:
	void UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt ) override;
	void CollisionResolution( std::unique_ptr<GameObject>& cube1, std::unique_ptr<GameObject>& cube2, float dt );

	bool useAABB = true;
	std::vector<std::unique_ptr<GameObject>> cubes;
};

#endif