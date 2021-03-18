#pragma once
#ifndef PARTICLEMODEL_H
#define PARTICLEMODEL_H

#include "../Graphics/Model/Transform.h"

class ParticleModel
{
public:
	ParticleModel() {}
	ParticleModel( std::shared_ptr<Transform> transform );
	std::shared_ptr<Transform> GetTransform() const noexcept { return _transform; }

	// collisions
	bool CollisionCheckAABB( v3df position );
	bool CollisionCheckCircle( v3df position, float radius );
	float GetCollisionRadius() const noexcept { return _boundingSphere; }
	void SetCollisionRadius( float radius ) noexcept { _boundingSphere = radius; }

	// particle movement/positioning
	void Move( float x, float y, float z ) noexcept { _netForce = { x, y, z }; }
	v3df GetPosition() const noexcept { return _position; }
	void SetPosition( v3df position ) noexcept { _position = position; }
	void SetPosition( float x, float y, float z ) noexcept { _position = { x, y, z }; }

	// particle state
	bool IsLaminar() const noexcept { return _useLaminar; }
	void SetLaminar( bool useLaminar ) noexcept { _useLaminar = useLaminar; }
	bool GetIsParticle() const noexcept { return _isParticle; }
	void SetIsParticle( bool isParticle ) noexcept { _isParticle = isParticle; }

	// get/set forces
	v3df GetNetForce() const { return _netForce; }
	v3df GetVelocity() const { return _velocity; }
	v3df GetAcceleration() const { return _acceleration; }

	void SetNetForce( v3df netForce ) noexcept { _netForce = netForce; }
	void SetNetForce( float x, float y, float z ) noexcept { _netForce = { x, y, z }; }
	void SetVelocity( v3df velocity ) noexcept { _velocity = velocity; }
	void SetVelocity( float x, float y, float z ) noexcept { _velocity = { x, y, z }; }
	void SetAcceleration( v3df acceleration ) noexcept { _acceleration = acceleration; }
	void SetAcceleration( float x, float y, float z ) noexcept { _acceleration = { x, y, z }; }

	// update constants
	float GetMass() const noexcept { return _mass; }
	float GetDragFactor() const noexcept { return _drag; }
	float GetInverseMass() const noexcept { return _inverseMass; }
	float GetFriction() const noexcept { return _frictionMultiplier; }

	void SetMass( float newMass ) noexcept { _mass = newMass; }
	void SetInverseMass( float value ) noexcept { _inverseMass = 1.0f / value; }
	void SetDragFactor( float dragFactor ) noexcept { _drag = dragFactor; }
	void SetFriction( float friction ) noexcept { _frictionMultiplier = friction; }

	// update forces
	virtual void Update( const float dt );
	void AddThrust( v3df force, float duration );
	void AddForce( v3df force ) noexcept { _netForce += force; }
	virtual void ResetForces();
protected:
	float _mass;
	float _drag;
	bool _useLaminar;
	bool _isParticle;
	float _inverseMass;

	v3df _friction;
	v3df _netForce;
	v3df _velocity;
	v3df _position;
	v3df _acceleration;

	std::shared_ptr<Transform> _transform;
private:
	// update forces
	void Weight();
	void ApplyThrust( const float dt );

	void Acceleration();
	void Friction( const float dt );
	void Velocity( const float dt );

	void DragForce( const float dt );
	void DragLaminar();
	void DragTurbulent();

	void ComputePosition( const float dt );
	void CheckWorldCollisions();

	// Constants
	float _gravity = 9.81f;
	float _limiter = 0.001f;
	float _boundingSphere = 1.0f;
	float _frictionMultiplier = 0.0002f;

	// Local Variables
	float _weight;
	std::vector<std::pair<v3df, float>> _thrustForces;
};

#endif