#pragma once
#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

#include "Transform.h"

const float GRAVITY = 9.81f;
const float TIME_STEP = 0.01f;
const float FRICTION = 0.00002f;
const float LIMITER = 0.01f;

class ParticleModel
{
public:
	ParticleModel( std::shared_ptr<Transform> transform, bool useConstAccel, v3df initialVelocity, v3df initialAccel );

	// particle movement/positioning
	void Move( float x, float y, float z );
	v3df GetEmitterPosition() const { return _emitterPosition; };
	void SetEmitterPosition( v3df position ) { _emitterPosition = position; };
	void SetEmitterPosition( float x, float y, float z ) { _emitterPosition = { x, y, z }; };

	// get forces
	v3df GetVelocity() const { return _velocity; }
	v3df GetAcceleration() const { return _acceleration; }
	bool GetConstAcceleration() const { return _useConstAccel; }

	// set forces
	void SetVelocity( v3df velocity ) { _velocity = velocity; };
	void SetVelocity( float x, float y, float z ) { _velocity = { x, y, z }; };
	void SetAcceleration( v3df acceleration ) { _acceleration = acceleration; }
	void SetAcceleration( float x, float y, float z ) { _acceleration = { x, y, z }; }
	void SetConstAcceleration ( bool accel ) { _useConstAccel = accel; }

	// update forces
	void Update( float deltaTime );
	void ComputeWeight();
	void ComputeAcceleration();
	void ComputeFriction();
	void ComputeVelocity();
	void ComputePosition();
	void CheckWorldCollisions();
	void ResetForces();
private:
	float _mass;
	float _weight;
	v3df _friction;
	v3df _netForce;
	v3df _velocity;
	v3df _acceleration;
	bool _useConstAccel;
	v3df _emitterPosition;
	std::shared_ptr<Transform> _transform;
};

#endif