#pragma once
#ifndef CONTACT_H
#define CONTACT_H

#include "../RigidBody.h"
#include "ContactResolver.h"

class Contact
{
	friend class ContactResolver;
public:
	void SetBodyData( RigidBody* a, RigidBody* b, float friction, float restitution )
	{
		_bodies[0] = a;
		_bodies[1] = b;
		_friction = friction;
		_restitution = restitution;
	}

	float _friction;
	float _restitution;
	float _penetration;

	v3df _contactPoint;
	v3df _contactNormal;
	RigidBody* _bodies[2];
protected:
	void CalculateInternals( float dt );
	void SwapBodies();
	void MatchAwakeState();
	void CalculateDesiredDeltaVelocity( float dt );
	v3df CalculateLocalVelocity( uint32_t bodyIndex, float dt );
	void CalculateContactBasis();
	void ApplyVelocityChange( v3df velocityChange[2], v3df rotationChange[2] );
	void ApplyPositionChange( v3df linearChange[2], v3df angularChange[2], float penetration );
	v3df CalculateFrictionlessImpulse( Matrix3* inverseIntertiaTensor );
	v3df CalculateFrictionImpulse( Matrix3* inverseInertiaTensor );

	Matrix3 _contactToWorld;
	v3df _contactVelocity;
	float _desiredDeltaVelocity;
	v3df _relativeContactPosition[2];
};

#endif