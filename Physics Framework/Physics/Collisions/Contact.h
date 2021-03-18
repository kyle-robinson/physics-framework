#pragma once
#ifndef CONTACT_H
#define CONTACT_H

#include "../RigidBody.h"
#include "ContactResolver.h"

class Contact
{
	friend class ContactResolver;
public:
	void SetBodyData( RigidBody* a, RigidBody* b, float friction, float restitution ) {
		_bodies[0] = a;
		_bodies[1] = b;
		_friction = friction;
		_restitution = restitution;
	}

	RigidBody* _bodies[2];

	float _friction;
	float _restitution;

	v3df _contactPoint;
	v3df _contactNormal;
	float _penetration;

protected:
	void CalculateInternals( float dt );
	void SwapBodies();
	void MatchAwakeState();
	void CalculateDesiredDeltaVelocity( float dt );
	v3df CalculateLocalVelocity( unsigned bodyIndex, float dt );
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