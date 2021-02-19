#pragma once
#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

#include "Transform.h"

class ParticleModel
{
public:
	ParticleModel() {}
	ParticleModel( std::shared_ptr<Transform> transform );
	std::shared_ptr<Transform> GetTransform() const noexcept { return _transform; }

	// particle movement/positioning
	void Move( float x, float y, float z );
	v3df GetEmitterPosition() const { return _emitterPosition; }
	void SetEmitterPosition( v3df position ) { _emitterPosition = position; }
	void SetEmitterPosition( float x, float y, float z ) { _emitterPosition = { x, y, z }; }

	// particle state
	bool IsLaminar() const noexcept { return _useLaminar; }
	void SetLaminar( bool useLaminar ) noexcept { _useLaminar = useLaminar; }
	bool GetIsParticle() const noexcept { return _isParticle; }
	void SetIsParticle( bool isParticle ) noexcept { _isParticle = isParticle; }

	// get/set forces
	v3df GetVelocity() const { return _velocity; }
	v3df GetAcceleration() const { return _acceleration; }

	void SetVelocity( v3df velocity ) { _velocity = velocity; }
	void SetVelocity( float x, float y, float z ) { _velocity = { x, y, z }; }
	void SetAcceleration( v3df acceleration ) { _acceleration = acceleration; }
	void SetAcceleration( float x, float y, float z ) { _acceleration = { x, y, z }; }

	// update constants
	float GetMass() const noexcept { return _mass; }
	float GetDragFactor() const noexcept { return _drag; }
	float GetFriction() const noexcept { return _frictionMultiplier; }

	void SetMass( float newMass ) { _mass = newMass; }
	void SetDragFactor( float dragFactor ) { _drag = dragFactor; }
	void SetFriction( float friction ) { _frictionMultiplier = friction; }

	// update forces
	virtual void Update( const float dt );
	
	void Weight();
	void ApplyThrust( const float dt );
	void AddThrust( v3df force, float duration );
	
	void Acceleration();
	void Friction( const float dt );
	void Velocity( const float dt );
	
	void DragForce( const float dt );
	void DragLaminar();
	void DragTurbulent();

	void ComputePosition( const float dt );
	void CheckWorldCollisions();

	void ResetForces();
private:
	// Constants
	float _gravity = 9.81f;
	float _limiter = 0.001f;
	float _frictionMultiplier = 0.0002f;

	// Local Variables
	float _drag;
	float _mass;
	float _weight;
	bool _useLaminar;
	bool _isParticle;

	v3df _friction;
	v3df _netForce;
	v3df _velocity;
	v3df _acceleration;
	v3df _emitterPosition;

	std::shared_ptr<Transform> _transform;
	std::vector<std::pair<v3df, float>> _thrustForces;
};

#endif