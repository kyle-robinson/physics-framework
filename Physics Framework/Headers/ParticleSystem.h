#pragma once
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "Particle.h"

class ParticleSystem
{
public:
	void Update( float deltaTime );
	void Draw();
private:
	Particle* p;
	int numParticles;
};

#endif