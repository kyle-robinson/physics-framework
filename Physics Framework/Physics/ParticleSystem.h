#pragma once
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#define MAX_PARTICLE_COUNT 300

#include "Particle.h"

class ParticleSystem
{
public:
	ParticleSystem( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureStone,
		Geometry geometry, Material material );
	void Update( float deltaTime );
	void Draw( ID3D11DeviceContext* pImmediateContext );
	void AddParticle( v3df position, v3df velocity, float maxAge );
	int FindNextAvailableParticleIndex();
private:
	Particle _particles[MAX_PARTICLE_COUNT];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureStone;
};

#endif