#pragma once
#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

#include "Transform.h"

#define GRAVITY 9.81f

//#define TIME_STEP 0.1f
//#define MOVEMENT_SPEED 0.0002f
//#define MAX_ACCELERATION 0.0001f

//#define NORMAL_FORCE 10000
//#define FRICTION_COEF 0.0000000013f

class ParticleModel
{
public:
	ParticleModel( std::shared_ptr<Transform> transform );

	// particle movement
	/*void const MoveForward();
	void const MoveBackward();
	void const MoveLeft();
	void const MoveRight();
	void const MoveUp();
	void MoveConstVelocity( const float deltaTime );
	void MoveConstAcceleration( const float deltaTime );*/

	// get forces
	float GetMass() const { return _mass; }
	float GetThrust() const { return _thrust; }
	v3df GetNetForce() const { return _netForce; }
	v3df GetVelocity() const { return _velocity; }
	v3df GetAcceleration() const { return _acceleration; }

	// set forces
	void SetMass( float mass ) { _mass = mass; }
	void SetThrust( float thrust ) { _thrust = thrust; }
	void SetNetForce( v3df netForce ) { _netForce = netForce; }
	void SetNetForce( float x, float y, float z ) { _netForce = { x, y, z }; }
	void SetVelocity( v3df velocity ) { _velocity = velocity; };
	void SetVelocity( float x, float y, float z ) { _velocity = { x, y, z }; };
	void SetAcceleration( v3df acceleration ) { _acceleration = acceleration; }
	void SetAcceleration( float x, float y, float z ) { _acceleration = { x, y, z }; }
	void SetEmitterPosition( v3df emitterPosition ) { _emitterPosition = emitterPosition; }
	void SetEmitterPosition( float x, float y, float z ) { _emitterPosition = { x, y, z }; }

	// update forces
	void Update( float deltaTime );
	void ApplyGravity( float deltaTime );
	void ApplyThrust( float deltaTime );

	void ComputeAcceleration( float deltaTime );
	void ComputeVelocity( float deltaTime );
	void ComputePosition( float deltaTime );

	void CheckFloorCollision();
private:
	v3df _force;
	float _mass;
	float _thrust;
	float _weight;
	v3df _netForce;
	v3df _velocity;
	v3df _friction;
	float _floorHeight;
	v3df _acceleration;
	bool _useConstAccel;
	v3df _emitterPosition;
	std::shared_ptr<Transform> _transform;
};

#endif