#include "stdafx.h"
#include "ContactResolver.h"

ContactResolver::ContactResolver( unsigned iterations, float velocityEps /*= (float)0.01*/, float positionEps /*= (float)0.01*/ )
{
	SetIterations( iterations );
	SetEpsilon( velocityEps, positionEps );
}

ContactResolver::ContactResolver( unsigned velocityIt, unsigned positionIt, float positionEp /*= (float)0.01*/, float velocityEp /*= (float)0.01*/ )
{
	SetIterations( positionIterations, velocityIterations );
	SetEpsilon( velocityEp, positionEp );
}

bool ContactResolver::IsValid()
{
	return( velocityIterations > 0 ) && ( positionIterations > 0 ) && ( positionEpsilon >= 0.0f ) && ( velocityEpsilon >= 0.0f );
}

void ContactResolver::SetIterations( unsigned velocityIt, unsigned positionIt )
{
	positionIterations = positionIt;
	velocityIterations = velocityIt;
}

void ContactResolver::SetIterations( unsigned iterations )
{
	positionIterations = iterations;
	velocityIterations = iterations;
}

void ContactResolver::SetEpsilon( float velocityEp, float positionEp )
{
	velocityEpsilon = velocityEp;
	positionEpsilon = positionEp;
}

void ContactResolver::ResolveContacts( Contact* contacts, unsigned numContacts, float dt )
{
	if ( numContacts == 0 ) return;
	if ( !IsValid() ) return;


	PrepareContacts( contacts, numContacts, dt );

	AdjustPositions( contacts, numContacts, dt );

	AdjustVelocities( contacts, numContacts, dt );
}

void ContactResolver::AdjustVelocities( Contact* contacts, unsigned numContacts, float dt )
{
	v3df velocityChange[2], rotationChange[2];
	v3df deltaVel;

	velocityIterationsUsed = 0;

	while ( velocityIterationsUsed < velocityIterations )
	{
		float max = velocityEpsilon;
		unsigned index = numContacts;
		for ( unsigned i = 0; i < numContacts; i++ ) {
			if ( contacts[i]._desiredDeltaVelocity > max ) {
				max = contacts[i]._desiredDeltaVelocity;
				index = i;
			}
		}
		if ( index == numContacts ) break;

		contacts[index].MatchAwakeState();
		contacts[index].ApplyVelocityChange( velocityChange, rotationChange );

		for ( unsigned i = 0; i < numContacts; i++ )
		{
			// Check each body in the contact
			for ( unsigned b = 0; b < 2; b++ ) if ( contacts[i]._bodies[b] )
			{
				// Check for a match with each body in the newly
				// resolved contact
				for ( unsigned d = 0; d < 2; d++ )
				{
					if ( contacts[i]._bodies[b] == contacts[index]._bodies[d] )
					{
						deltaVel = velocityChange[d] +
							rotationChange[d].VectorProduct(
								contacts[i]._relativeContactPosition[b] );

						// The sign of the change is negative if we're dealing
						// with the second body in a contact.
						contacts[i]._contactVelocity +=
							contacts[i]._contactToWorld.TransformTranspose( deltaVel )
							* ( b ? -1 : 1 );
						contacts[i].CalculateDesiredDeltaVelocity( dt );
					}
				}
			}
		}
		velocityIterationsUsed++;
	}
}

void ContactResolver::AdjustPositions( Contact* contacts, unsigned numContacts, float dt )
{
	unsigned i, index;
	v3df linearChange[2], angularChange[2];
	float max;
	v3df deltaPosition;


	positionIterationsUsed = 0;
	while ( positionIterationsUsed < positionIterations ) {

		//Find the biggest penetration
		max = positionEpsilon;
		index = numContacts;
		for ( i = 0; i < numContacts; i++ ) {
			if ( contacts[i]._penetration > max ) {
				max = contacts[i]._penetration;
				index = i;
			}
		}

		if ( index == numContacts ) break;

		contacts[index].MatchAwakeState();

		contacts[index].ApplyPositionChange( linearChange, angularChange, max );

		for ( i = 0; i < numContacts; i++ )
		{
			// Check each _bodies in the contact
			for ( unsigned b = 0; b < 2; b++ ) if ( contacts[i]._bodies[b] )
			{
				// Check for a match with each _bodies in the newly
				// resolved contact
				for ( unsigned d = 0; d < 2; d++ )
				{
					if ( contacts[i]._bodies[b] == contacts[index]._bodies[d] )
					{
						deltaPosition = linearChange[d] +
							angularChange[d].VectorProduct(
								contacts[i]._relativeContactPosition[b] );

						// The sign of the change is positive if we're
						// dealing with the second _bodies in a contact
						// and negative otherwise (because we're
						// subtracting the resolution)..
						contacts[i]._penetration +=
							deltaPosition.ScalarProduct( contacts[i]._contactNormal )
							* ( b ? 1 : -1 );
					}
				}
			}
		}
		positionIterationsUsed++;
	}
}

void ContactResolver::PrepareContacts( Contact* contacts, unsigned numContacts, float dt )
{
	Contact* lastContact = contacts + numContacts;

	for ( Contact* contact = contacts; contact < lastContact; contact++ ) {
		contact->CalculateInternals( dt );
	}
}