#pragma once
#ifndef PARTICLE_H
#define PARTICLE_H

#include "Transform.h"
#include "ParticleModel.h"

class Particle
{
public:
	Particle();
	void Update( float deltaTime );
	void Spawn( v3df position, v3df velocity, float maxAge );

	bool GetActive() const noexcept { return _active; };
	float GetAge() const noexcept { return _curAge; };
	float GetMaxAge() const noexcept { return _maxAge; };

	std::shared_ptr<Transform> GetTransform() const noexcept { return _transform; };
	std::shared_ptr<ParticleModel> GetParticleModel() const noexcept { return _particleModel; };
private:
	int _curAge;
	int _maxAge;
	bool _active;
	std::shared_ptr<Transform> _transform;
	std::shared_ptr<ParticleModel> _particleModel;
};

#endif