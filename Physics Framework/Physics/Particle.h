#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include "../Graphics/Model/Transform.h"
#include "../Graphics/Model/Appearance.h"
#include "ParticleModel.h"

class Particle
{
public:
	Particle( const std::string& id );
	void Update( const float dt );
	void Draw( ID3D11DeviceContext* pImmediateContext );
	void RespawnParticle();

	float GetMaxSize() const noexcept { return size; };
	void SetMaxSize( float newSize ) { size = newSize; };

	int GetStartTimer() const noexcept { return startTimer; };
	const std::string& GetID() const noexcept { return id; };

	std::shared_ptr<Transform> GetTransform() const noexcept { return _transform; };
	std::shared_ptr<Appearance> GetAppearance() const noexcept { return _appearance; };
	std::shared_ptr<ParticleModel> GetParticleModel() const noexcept { return _particleModel; };
private:
	// Local Variables
	float size;
	int energy;
	int maxEnergy;
	int startTimer;
	std::string id;

	// Class References
	std::shared_ptr<Transform> _transform;
	std::shared_ptr<Appearance> _appearance;
	std::shared_ptr<ParticleModel> _particleModel;
};

#endif