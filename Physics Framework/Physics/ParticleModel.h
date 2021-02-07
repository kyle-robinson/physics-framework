#pragma once
#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

#include "Transform.h"

class ParticleModel
{
public:
	ParticleModel() {};
	ParticleModel( std::shared_ptr<Transform> transform );

	// particle movement/positioning
	void Move( float x, float y, float z );
	v3df GetEmitterPosition() const { return _emitterPosition; };
	void SetEmitterPosition( v3df position ) { _emitterPosition = position; };
	void SetEmitterPosition( float x, float y, float z ) { _emitterPosition = { x, y, z }; };

	// particle state
	bool IsLaminar() const { return _useLaminar; };
	void SetLaminar( bool useLaminar ) { _useLaminar = useLaminar; };

	// get/set forces
	v3df GetVelocity() const { return _velocity; }
	void SetVelocity( v3df velocity ) { _velocity = velocity; };
	void SetVelocity( float x, float y, float z ) { _velocity = { x, y, z }; };
	v3df GetAcceleration() const { return _acceleration; }
	void SetAcceleration( v3df acceleration ) { _acceleration = acceleration; }
	void SetAcceleration( float x, float y, float z ) { _acceleration = { x, y, z }; }

	// update constants
	float GetTimeStep() const { return TIME_STEP; };
	void SetTimeStep( float timeStep ) { TIME_STEP = timeStep; };
	float GetGravity() const noexcept { return GRAVITY; };
	void SetGravity( float gravity ) { GRAVITY = gravity; };
	float GetDragFactor() const noexcept { return DRAG_FACTOR; };
	void SetDragFactor( float dragFactor ) { DRAG_FACTOR = dragFactor; };
	float GetFriction() const noexcept { return FRICTION; };
	void SetFriction( float friction ) { FRICTION = friction; };
	float GetMass() const noexcept { return _mass; };
	void SetMass( float newMass ) { _mass = newMass; };

	// update forces
	virtual void Update( const float dt );
	void Weight();
	void DragForce();
	void DragLaminar();
	void DragTurbulent();
	void Acceleration();
	void Friction();
	void Velocity();
	void ComputePosition();
	void CheckWorldCollisions();
	void ResetForces();
private:
	// Constants
	float GRAVITY = 9.81f;
	float TIME_STEP = 0.5f;
	float LIMITER = 0.001f;
	float DRAG_FACTOR = 5.0f;
	float FRICTION = 0.0002f;

	// Local Variables
	float _mass;
	float _weight;
	bool _useLaminar;
	v3df _drag;
	v3df _friction;
	v3df _netForce;
	v3df _velocity;
	v3df _acceleration;
	v3df _emitterPosition;
	std::shared_ptr<Transform> _transform;
};

#endif