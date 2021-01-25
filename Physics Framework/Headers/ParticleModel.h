#pragma once
#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

class Transform;

class ParticleModel
{
public:
	ParticleModel( Transform* transform, bool useConstAccel, v3df initialVelocity, v3df initialAcceleration );
	void Update( float deltaTime );

	v3df GetVelocity() { return _velocity; }
	void SetVelocity( v3df velocity ) { _velocity = velocity; };
	
	void MoveConstVelocity( const float deltaTime );
	void MoveConstAcceleration( const float deltaTime );
private:
	v3df _velocity;
	v3df _acceleration;
	bool _useConstVelocity;
	std::unique_ptr<Transform> _transform;
};

#endif