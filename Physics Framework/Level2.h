#pragma once
#ifndef LEVEL2_H
#define LEVEL2_H

#include "LevelManager.h"

class Level2 : public LevelManager
{
public:
	void Initialize( Graphics& gfx ) override;
	void Update( float dt ) override;
	void Render( Graphics& gfx ) override;
	void SpawnControlWindow( std::vector<std::unique_ptr<Particle>>& vec );
private:
	float xDist = 2.0f;
	bool useParticles = false;
	std::vector<std::unique_ptr<Particle>> particles;
};

#endif