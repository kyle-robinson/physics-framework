#pragma once
#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#define MAX_PARTICLE_COUNT 300

#include "Particle.h"

class ParticleSystem;
class ParticleEmitter
{
public:
	ParticleEmitter( ParticleSystem* particleSystem, v3df position, v3df direction, float directionRandomness,
		float power, float powerRandomModifier, float maxAge, float maxAgeRandomModifier,
		float spawnDelay, float spawnDelayRandomModifier );
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

	ParticleSystem* _particleSystem;
};

class ParticleSystem
{
public:
	ParticleSystem();
	void Update( float deltaTime );
	void Draw();
	void AddParticle( v3df position, v3df velocity, float maxAge );
	int FindNextAvailableParticleIndex();
	void AddEmitter( v3df position, v3df direction, float directionRandomness, float power, float powerRandomModifier,
		float maxAge, float maxAgeRandomModifier, float spawnDelay, float spawnDelayRandomModifier );
	std::vector<ParticleEmitter*> GetEmitters() const noexcept { return _particleEmitters; };
private:
	void DestroyParticle( int index );
	std::vector<Particle> _particles;
	std::vector<ParticleEmitter*> _particleEmitters;
};

#endif