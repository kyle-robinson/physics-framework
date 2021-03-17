#pragma once
#ifndef LEVEL2_H
#define LEVEL2_H

#include "LevelManager.h"
#include "Particle.h"

#define PARTICLE_COUNT 20

class Level2 : public LevelManager
{
public:
	void Initialize( Graphics& gfx ) override;
	void Update( Mouse& mouse, Keyboard& keyboard, float dt ) override;
	void Render( Graphics& gfx ) override;
	void SpawnControlWindow( std::vector<std::unique_ptr<Particle>>& vec );
private:
	void UpdateInput( Mouse& mouse, Keyboard& keyboard, float dt ) override;

	float xDist = 2.0f;
	bool useParticles = true;
	std::vector<std::unique_ptr<Particle>> particles;
};

#endif