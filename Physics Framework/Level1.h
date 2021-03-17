#pragma once
#ifndef LEVEL1_H
#define LEVEL1_H

#include "LevelManager.h"
class Keyboard;

class Level1 : public LevelManager
{
public:
	void Initialize( Graphics& gfx ) override;
	void Update( float dt ) override;
	void UpdateInput( Keyboard& keyboard );
	void Render( Graphics& gfx ) override;
	void SpawnControlWindow( std::vector<std::unique_ptr<GameObject>>& vec );
private:
	void CollisionResolution( std::unique_ptr<GameObject>& cube1, std::unique_ptr<GameObject>& cube2, float dt );

	bool useAABB = true;
	std::vector<std::unique_ptr<GameObject>> cubes;
};

#endif