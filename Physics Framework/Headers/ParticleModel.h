#pragma once
#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

#include "Transform.h"

class ParticleModel
{
public:
	ParticleModel( std::shared_ptr<Transform> transform );

	v3df GetVelocity() const { return _velocity; }
	void SetVelocity( v3df velocity ) { _velocity = velocity; };

	v3df GetAcceleration() const { return _acceleration; }
	void SetAcceleration( v3df acceleration ) { _acceleration = acceleration; }

	float GetMass() const { return _mass; }
	void SetMass( float mass ) { _mass = mass; }

	float GetNetForce() const { return _netForce; }
	void SetNetForce( float netForce ) { _netForce = netForce; }
	
	void MoveConstVelocity( const float deltaTime );
	void MoveConstAcceleration( const float deltaTime );
private:
	float _mass;
	float _netForce;
	v3df _velocity;
	v3df _acceleration;
	bool _useConstAccel;
	std::shared_ptr<Transform> _transform;
};

#endif