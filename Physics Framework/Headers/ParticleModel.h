#pragma once
#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

#include "GameObject.h"

class ParticleModel : public GameObject
{
public:
	//ParticleModel() {}
	ParticleModel( const std::string& name, bool usePhysics, bool useConstAccel );
	ParticleModel( const std::string& name, bool usePhysics, bool useConstAccel, v3df initialVelocity, v3df initialAcceleration );
	
	void Update( float deltaTime ) override;
	void Draw( ID3D11DeviceContext* pImmediateContext ) override;

	v3df GetVelocity() { return _velocity; }
	void SetVelocity( v3df velocity ) { _velocity = velocity; };
	
	void MoveConstVelocity( const float deltaTime );
	void MoveConstAcceleration( const float deltaTime );
private:
	v3df _velocity;
	v3df _acceleration;
	bool _useConstAccel;
	bool _usePhysics;
};

#endif