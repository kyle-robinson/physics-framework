#pragma once
#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

#include "Transform.h"

#define GRAVITY -9.81f
#define TIME_STEP 0.01f
#define LIMITER 0.0000001f
#define WORLD_BOUNDARY 15.0f

class ParticleModel
{
public:
	ParticleModel( std::shared_ptr<Transform> transform, bool useConstAccel, v3df initialVelocity, v3df initialAccel );

	// particle movement
	void Move( float x, float y, float z );
	//void MoveConstVelocity( float deltaTime );
	//void MoveConstAcceleration( float deltaTime );

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
	void ApplyGravity();
	void ComputeAcceleration( float deltaTime );
	void ComputeVelocity();
	void ComputePosition();
	void CheckWorldCollisions();
	void ResetForces();
private:
	float _mass;
	//v3df _force;
	v3df _netForce;
	v3df _velocity;
	v3df _acceleration;
	bool _useConstAccel;
	std::shared_ptr<Transform> _transform;
};

#endif