#pragma once
#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include "Transform.h"
#include "ParticleModel.h"
#include "ParticleSystem.h"

class ParticleEmitter
{
public:
	ParticleEmitter( std::shared_ptr<ParticleSystem> particleSystem,
		v3df position, v3df direction, float directionRandomness, float power, float powerRandomModifier,
		float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier );
	void Update( float deltaTime );
	void SpawnParticle();

	v3df GetPosition() const noexcept { return _position; };
	v3df GetDirection() const noexcept { return _direction; };

	float GetDirectionRandomness() const noexcept { return _directionRandomness; };
	
	float GetPower() const noexcept { return _power; };
	float GetPowerRandomModifier() const noexcept { return _powerRandomModifier; };

	float GetMaxAge() const noexcept { return _maxAge; };
	float GetMaxAgeRandomModifier() const noexcept { return _maxAgeRandomModifier; };

	float GetSpawnDelay() const noexcept { return _spawnDelay; };
	float GetSpawnDelayRandomModifier() const noexcept { return _spawnDelayRandomModifier; };
private:
	v3df _position;
	v3df _direction;

	float _spawnTimer;
	float _directionRandomness;

	float _power;
	float _powerRandomModifier;

	float _maxAge;
	float _maxAgeRandomModifier;

	float _spawnDelay;
	float _spawnDelayRandomModifier;

	std::shared_ptr<ParticleSystem> _particleSystem;
};

#endif