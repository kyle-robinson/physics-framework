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
	
	void MoveConstVelocity( const float deltaTime );
	void MoveConstAcceleration( const float deltaTime );
public:
	v3df _velocity;
	v3df _acceleration;
	bool _useConstAccel;
private:
	std::shared_ptr<Transform> _transform;
};

#endif