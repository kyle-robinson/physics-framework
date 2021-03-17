#pragma once
#ifndef LEVEL3_H
#define LEVEL3_H

#include "LevelManager.h"
#include "Contact.h"
#include "CollisionData.h"
#include "ContactResolver.h"
#include "CollisionDetector.h"
#include "CollisionPrimitives.h"

#define MAX_CONTACTS 10
#define NUMBER_OF_RIGID_CUBES 2

class Level3 : public LevelManager
{
public:
	void Initialize( Graphics& gfx ) override;
	void Update( Mouse& mouse, Keyboard& keyboard, float dt ) override;
	void Render( Graphics& gfx ) override;
	void SpawnControlWindow();
private:
	void UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt ) override;

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