#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include "Transform.h"
#include "Appearance.h"
#include "ParticleModel.h"
#include "ConstantBuffer.h"
//#include "ParticleSystem.h"

class Particle
{
public:
	Particle( const std::string& id );
	void Update();
	void Draw( ID3D11DeviceContext* pImmediateContext, ConstantBuffer<CB_VS_matrix>& cb_vs_matrix );
	//void Respawn( v3df position, v3df velocity, float maxAge );
	void Respawn();

	float GetMaxEnergy() const noexcept { return MAX_ENERGY; };
	void SetMaxEnergy( float newEnergy ) { MAX_ENERGY = newEnergy; };
	float GetSize() const noexcept { return SIZE; };
	void SetSize( float size ) { SIZE = size; };
	//float GetMaxLife() const noexcept { return LIFE; };
	//void SetMaxLife( float newMaxLife ) { LIFE = newMaxLife; };

	int GetStartTimer() const noexcept { return startTimer; };
	const std::string& GetID() const noexcept { return id; };

	std::shared_ptr<Transform> GetTransform() const noexcept { return _transform; };
	std::shared_ptr<Appearance> GetAppearance() const noexcept { return _appearance; };
	std::shared_ptr<ParticleModel> GetParticleModel() const noexcept { return _particleModel; };
private:
	// Constants
	//float LIFE = 0.1f;
	float SIZE = 0.01f;
	int MAX_ENERGY = 100;

	// Local Variables
	int energy;
	int startTimer;
	std::string id;

	// Class References
	std::shared_ptr<Transform> _transform;
	std::shared_ptr<Appearance> _appearance;
	std::shared_ptr<ParticleModel> _particleModel;
};

#endif