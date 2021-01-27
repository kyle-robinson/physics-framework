#pragma once
#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

#include "Transform.h"

//#define GRAVITY 0.1f
#define TIME_STEP 0.1f
#define MOVEMENT_SPEED 0.0002f
//#define NORMAL_FORCE 250
//#define FRICTION_COEF 0.13f

class ParticleModel
{
public:
	ParticleModel( std::shared_ptr<Transform> transform );

	void const MoveForward();
	void const MoveBackward();
	void const MoveLeft();
	void const MoveRight();

	v3df GetVelocity() const { return _velocity; }
	void SetVelocity( v3df velocity ) { _velocity = velocity; };
	void SetVelocity( float x, float y, float z ) { _velocity = { x, y, z }; };

	v3df GetAcceleration() const { return _acceleration; }
	void SetAcceleration( v3df acceleration ) { _acceleration = acceleration; }
	void SetAcceleration( float x, float y, float z ) { _acceleration = { x, y, z }; }

	float GetMass() const { return _mass; }
	void SetMass( float mass ) { _mass = mass; }

	v3df GetNetForce() const { return _netForce; }
	void SetNetForce( v3df netForce ) { _netForce = netForce; }
	void SetNetForce( float x, float y, float z ) { _netForce = { x, y, z }; }
	
	void MoveConstVelocity( const float deltaTime );
	void MoveConstAcceleration( const float deltaTime );

	void UpdateState();
	void UpdateNetForce();
	void UpdateAcceleration();
	void Move();
private:
	v3df _force;
	float _mass;
	v3df _netForce;
	v3df _velocity;
	float _friction;
	v3df _acceleration;
	bool _useConstAccel;
	std::shared_ptr<Transform> _transform;
};

#endif