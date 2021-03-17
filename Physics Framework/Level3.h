#pragma once
#ifndef LEVEL3_H
#define LEVEL3_H

#include "LevelManager.h"

class Level3 : public LevelManager
{
public:
	void Initialize( Graphics& gfx ) override;
	void Update( float dt ) override;
	void Render( Graphics& gfx ) override;
	void SpawnControlWindow();
private:
	// Rigid Bodies
	Box* topCube;
	Box* bottomCube;
	CollisionPlane* collisionPlane;

	CollisionData collisionData;
	ContactResolver* contactResolver;
	Contact contacts[MAX_CONTACTS];

	std::vector<std::unique_ptr<GameObject>> rigidCubes;
};

#endif