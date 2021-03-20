#pragma once
#ifndef LEVEL3_H
#define LEVEL3_H

#include "LevelManager.h"
#include "../Physics/Collisions/Contact.h"
#include "../Physics/Collisions/CollisionData.h"
#include "../Physics/Collisions/ContactResolver.h"
#include "../Physics/Collisions/CollisionDetector.h"
#include "../Physics/Collisions/CollisionPrimitives.h"

#define MAX_CONTACTS 10
#define NUMBER_OF_RIGID_CUBES 2

class Level3 : public LevelManager
{
private:
	enum ActiveSimulation
	{
		SIMULATION_1,
		SIMULATION_2,
		SIMULATION_3,
		SIMULATION_4
	} activeSimulation = SIMULATION_1;
public:
	void Initialize( Graphics& gfx ) override;
	void Update( Mouse& mouse, Keyboard& keyboard, float dt ) override;
	void Render( Graphics& gfx ) override;
private:
	void SpawnControlWindow();
	void SpawnInstructionWindow();
	void SpawnSimulationWindow( Graphics& gfx );
	void UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt ) override;
	void LoadSimulation( ActiveSimulation simulation );
	void StopSimulation();

	// Rigid Bodies
	float friction = 0.9f;
	float restitution = 0.1f;
	float tolerance = 0.1f;

	std::unique_ptr<CollisionCube> topCube;
	std::unique_ptr<CollisionCube> bottomCube;
	std::unique_ptr<CollisionPlane> collisionPlane;

	CollisionData collisionData;
	Contact contacts[MAX_CONTACTS];
	std::unique_ptr<ContactResolver> contactResolver;

	std::vector<std::unique_ptr<GameObject>> rigidCubes;
};

#endif