#pragma once
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#define MAX_PARTICLE_COUNT 300

#include "Particle.h"

class ParticleSystem
{
public:
	ParticleSystem();
	void Update( float deltaTime );
	void Draw();
	void AddParticle( v3df position, v3df velocity, float maxAge );
	int FindNextAvailableParticleIndex();
private:
	void DestroyParticle( int index );
	std::vector<Particle> _particles;
};

#endif