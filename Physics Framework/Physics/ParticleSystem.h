#pragma once
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

/*#define MAX_PARTICLE_COUNT 10

//#include "Particle.h"

//#include "Particle.h"
#include "Transform.h"
#include "Appearance.h"
#include "ConstantBuffer.h"

class Particle;
class ParticleSystem
{
public:
	//ParticleSystem() { _particles.resize( MAX_PARTICLE_COUNT ); }
	ParticleSystem() {};
	ParticleSystem( Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, Geometry geometry, Material material );
	virtual void Update();
	virtual void Draw( ID3D11DeviceContext* pImmediateContext, ConstantBuffer<CB_VS_matrix>& cb_vs_matrix );
	//void AddParticle( v3df position, v3df velocity, float maxAge );
	//int FindNextAvailableParticleIndex();
private:
	int numAlive;
	Particle _particles[MAX_PARTICLE_COUNT];
	//std::vector<std::unique_ptr<Particle>> _particles;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
};*/

#endif